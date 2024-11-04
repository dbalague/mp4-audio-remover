#include <iostream>
#include <filesystem>
#include <string>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
}


namespace fs = std::filesystem;

int remove_audio_from_video(const std::string& input_filename, const std::string& output_filename) {

    AVFormatContext* input_format_context = nullptr;

    // Open input file
    if (avformat_open_input(&input_format_context, input_filename.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Could not open input file: " << input_filename << "\n";
        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(input_format_context, nullptr) < 0) {
        std::cerr << "Could not find stream information for file: " << input_filename << "\n";
        avformat_close_input(&input_format_context);
        return -1;
    }

    // Create output context
    AVFormatContext* output_format_context = nullptr;
    if (avformat_alloc_output_context2(&output_format_context, nullptr, nullptr, output_filename.c_str()) < 0) {
        std::cerr << "Could not create output context for file: " << output_filename << "\n";
        avformat_close_input(&input_format_context);
        return -1;
    }

    // Add all video streams from input to output
    for (unsigned int i = 0; i < input_format_context->nb_streams; i++) {
        AVStream* in_stream = input_format_context->streams[i];
        if (in_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVStream* out_stream = avformat_new_stream(output_format_context, nullptr);
            if (!out_stream) {
                std::cerr << "Failed allocating output stream for file: " << output_filename << "\n";
                avformat_close_input(&input_format_context);
                avformat_free_context(output_format_context);
                return -1;
            }

            // Copy codec parameters from input to output stream
            if (avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar) < 0) {
                std::cerr << "Failed to copy codec parameters for file: " << output_filename << "\n";
                avformat_close_input(&input_format_context);
                avformat_free_context(output_format_context);
                return -1;
            }
            out_stream->codecpar->codec_tag = 0;
        }
    }

    // Open output file
    if (!(output_format_context->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&output_format_context->pb, output_filename.c_str(), AVIO_FLAG_WRITE) < 0) {
            std::cerr << "Could not open output file: " << output_filename << "\n";
            avformat_close_input(&input_format_context);
            avformat_free_context(output_format_context);
            return -1;
        }
    }

    // Write output file header
    if (avformat_write_header(output_format_context, nullptr) < 0) {
        std::cerr << "Error occurred when writing output file header for file: " << output_filename << "\n";
        avformat_close_input(&input_format_context);
        avio_closep(&output_format_context->pb);
        avformat_free_context(output_format_context);
        return -1;
    }

    AVPacket packet;
    while (av_read_frame(input_format_context, &packet) >= 0) {
        AVStream* in_stream = input_format_context->streams[packet.stream_index];

        // Only keep video packets
        if (in_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVStream* out_stream = output_format_context->streams[packet.stream_index];

            // Convert PTS/DTS
	    packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

            packet.duration = av_rescale_q(packet.duration, in_stream->time_base, out_stream->time_base);
            packet.pos = -1;

            // Write packet
            if (av_interleaved_write_frame(output_format_context, &packet) < 0) {
                std::cerr << "Error muxing packet for file: " << output_filename << "\n";
                break;
            }
        }

        av_packet_unref(&packet);
    }

    // Write file trailer
    av_write_trailer(output_format_context);

    // Clean up
    avformat_close_input(&input_format_context);
    if (!(output_format_context->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&output_format_context->pb);
    }
    avformat_free_context(output_format_context);

    std::cout << "Audio removed successfully, and video saved to " << output_filename << "\n";
    return 0;
}


// Utility function to convert a string to lowercase
std::string to_lowercase(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower_str;
}


int main() {
    std::string extension = ".mp4";

    std::cout << "Processing directory: " << fs::current_path() << "\n";

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.is_regular_file()) {
            // Get the file extension and convert it to lowercase
            std::string file_extension = to_lowercase(entry.path().extension().string());

            if (file_extension == extension) {
                std::string input_filename = entry.path().string();
                std::string output_filename = entry.path().stem().string() + "_noaudio" + extension;

                std::cout << "Processing file: " << input_filename << "\n";
                int result = remove_audio_from_video(input_filename, output_filename);
                if (result != 0) {
                    std::cerr << "Failed to process file: " << input_filename << "\n";
                } else {
                    std::cout << "Processed file successfully: " << input_filename << "\n";
                }
            }
        }
    }

    return 0;
}
