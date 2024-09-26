#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>

using boost::asio::ip::udp;

struct UDPObjectData
{
    uint64_t timestamp{ 123 };                                      // Default
    uint64_t uid{ 1 };                                              // Sensor specific labeling
    char uri[50]{ "lidar" };                                    // Sensor name
    uint32_t id{ 1 };                                               // Index designation of sensor
    uint32_t id_int{ std::numeric_limits<uint32_t>::max() };        // Index designation given by reading function
    char type[50]{ "Human" };                                    // Object Type
    cv::Vec3f center{ cv::Vec3f() };                                 // 3D coordinates center of gravity
    cv::Vec3f bb3DMin{ cv::Vec3f() };                                // 3D coordinates minimum corner point
    cv::Vec3f bb3DMax{ cv::Vec3f() };                                // 3D coordinates maximum corner point
};

struct UDPObjectsData {
    std::array<UDPObjectData, 25> udpObjectsList;
    size_t validCount{ 0 };
};

void receive_udp_data(const std::string& ip, uint16_t port) {
    try {
        boost::asio::io_context io_context;

        udp::endpoint local_endpoint(boost::asio::ip::make_address(ip), port);
        udp::socket socket(io_context, local_endpoint);

        boost::asio::socket_base::receive_buffer_size option(65536); 
        socket.set_option(option);
        std::cout << "Listening for UDP data on " << ip << ":" << port << std::endl;

        while (true) {
            UDPObjectsData udpData;
            udp::endpoint sender_endpoint;

           
            size_t receivedBytes = socket.receive_from(boost::asio::buffer(&udpData, sizeof(UDPObjectsData)), sender_endpoint);

            std::cout << "Received " << receivedBytes << " bytes from " << sender_endpoint.address().to_string() << ":" << sender_endpoint.port() << std::endl;

            if (receivedBytes < sizeof(UDPObjectsData)) {
                std::cerr << "Received incomplete UDP packet. Expected " << sizeof(UDPObjectsData) << " bytes, received " << receivedBytes << " bytes." << std::endl;
                continue;  
            }
            std::cout << "Valid count: " << udpData.validCount << std::endl;
            // Process received data
            for (size_t i = 0; i < udpData.validCount; ++i) {
                const UDPObjectData& obj = udpData.udpObjectsList[i];
                std::cout << "Timestamp: " << obj.timestamp << std::endl;
                std::cout << "UID: " << obj.uid << std::endl;
                std::cout << "URI: " << obj.uri << std::endl;
                std::cout << "ID: " << obj.id << std::endl;
                std::cout << "ID Int: " << obj.id_int << std::endl;
                std::cout << "Type: " << obj.type << std::endl;
                std::cout << "Center: (" << obj.center[0] << ", " << obj.center[1] << ", " << obj.center[2] << ")" << std::endl;
                std::cout << "bb3DMin: (" << obj.bb3DMin[0] << ", " << obj.bb3DMin[1] << ", " << obj.bb3DMin[2] << ")" << std::endl;
                std::cout << "bb3DMax: (" << obj.bb3DMax[0] << ", " << obj.bb3DMax[1] << ", " << obj.bb3DMax[2] << ")" << std::endl;
                std::cout << std::endl;
            }

            std::cout << "End of object list" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port>" << std::endl;
        return 1;
    }

    const std::string ip = argv[1];
    const uint16_t port = static_cast<uint16_t>(std::stoi(argv[2]));

    receive_udp_data(ip, port);

    return 0;
}
