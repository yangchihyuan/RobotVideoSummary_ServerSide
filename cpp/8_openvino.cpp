/* Chih-Yuan Yang 2024
This is the main function, which launchs three threads. One receives frames from Zenbo.
The second sends control signal back to Zenbo.
The third processes images.
*/

#include <vector>
#include <thread>       //without this, std::thread is unknown


#include "8_openvino.hpp"
#include "SocketToServer.hpp"
#include "ProcessImage.hpp"
#include "utility_directory.hpp"
#include "utility_string.hpp"
#include "utility_csv.hpp"


bool ParseAndCheckCommandLine(int argc, char* argv[]) {
    // ---------------------------Parsing and validation of input args--------------------------------------

    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    std::cout << "[ INFO ] Parsing input parameters" << std::endl;
    return true;
}

int main(int argc, char* argv[]) {

    try {
        // ------------------------------ Parsing and validation of input args ---------------------------------
        if (!ParseAndCheckCommandLine(argc, argv)) {   
            return EXIT_SUCCESS;
        }

        std::thread thread_receive_frame(receive_socket, FLAGS_port_number);
        std::thread thread_report_results(report_results, FLAGS_port_number+1);     //another thread to send back results.
        std::thread thread_image_process(process_image, 
            FLAGS_pose_model, 
            FLAGS_ShowRenderedImage, 
            FLAGS_SaveTransmittedImage, 
            FLAGS_save_to_directory//,
        );

        thread_receive_frame.join();
        thread_report_results.join();
        thread_image_process.join();
    }
    catch (const std::exception& error) {
        std::cerr << "[ ERROR ] " << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "[ ERROR ] Unknown/internal exception happened." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[ INFO ] Execution successful" << std::endl;
    return EXIT_SUCCESS;
}
