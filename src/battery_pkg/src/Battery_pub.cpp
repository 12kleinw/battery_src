#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <battery_pkg/Battery.h>

#define STARTFLAG 0xA5
#define UPPERADD 0x40
#define BMSADD 0x01
#define ELECTID 0x90
#define STATUSID 0x93
#define ERRORID 0x98
#define DATALENGTH 0x08


// //创建一个电池的数据信息结构体
// struct Packet {
//     //帧头
//     uint8_t startFlag;
//     //通信地址
//     uint8_t address;
//     //数据ID
//     uint8_t dataID;
//     //数据长度
//     uint8_t dataLength;
//     //数据内容
//     std::vector<uint8_t> data;
//     //校验和
//     uint8_t checksum;
// };

// uint8_t calculate_checksum(const Packet &packet)
// {
//     uint8_t checksum = 0;
//     for(size_t i = 0; i <sizeof(packet.startFlag) + sizeof(packet.address) + sizeof(packet.dataID) + sizeof(packet.dataLength) + packet.data.size(); ++i){
//         uint8_t byte = (i == 0) ? packet.startFlag :
//                         (i == 1) ? packet.address :
//                         (i == 2) ? packet.dataID :
//                         (i == 3) ? packet.dataLength :
//                         (i >= 4 && i < 4 + packet.data.size()) ?
// packet.data[i - 4] : 0;
//     checksum += byte;
//     }
//     return checksum;
// }

// void setChecksum(Packet & packet)
// {
//     packet.checksum = calculate_checksum(packet);
// }

int main(int argc,char *argv[])
{
    //编码
    setlocale(LC_ALL,"");
    //创建ros节点
    ros::init(argc,argv,"battery_port");
    //创建句柄
    ros::NodeHandle nh;
    //设置发布电量的话题
    ros::Publisher battery_pub = nh.advertise<battery_pkg::Battery>("battery_pub.msg",10);
    battery_pkg::Battery Battery;
    //设置发布电池状态的话题
    //ros::Publisher battery_status = nh.advertise<battery_pkg::Battery>("battery_status_pub",10);
    //设置发布电池故障的话题
    ros::Publisher battery_error = nh.advertise<std_msgs::String>("YHS_CIR02/Error",10);
    std_msgs::String msg;
    //创建一个serial类
    serial::Serial sp;
    //创建一个timeout
    serial::Timeout to = serial::Timeout::simpleTimeout(100);
    //设置要打开的串口名称
    sp.setPort("/dev/ttyS3");
    //设置串口通信的波特率
    sp.setBaudrate(9600);
    sp.setTimeout(to);
    try
    {
        sp.open();
    }
    catch(const std::exception& e)
    {
        ROS_ERROR_STREAM("Unable to open port");
        return -1;
    }
    if(sp.isOpen())
    {
        ROS_INFO_STREAM("/dev/ttyS3 is opended");
    }
    else
    {
        return -1;
    }
    ros::Rate loop_rate(500);
    //创建查询电量的数据帧
    // Packet electrictity_packet;
    // electrictity_packet.startFlag = STARTFLAG;
    // electrictity_packet.address = UPPERADD;
    // electrictity_packet.dataID = ELECTID;
    // electrictity_packet.dataLength = DATALENGTH;
    // electrictity_packet.data = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    // setChecksum(electrictity_packet);
    uint8_t electArray[14] ={0xA5,0x40,0x90,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7D};
    //创建查询电池状态的数据帧
    // Packet status_packet;
    // status_packet.startFlag = STARTFLAG;
    // status_packet.address = UPPERADD;
    // status_packet.dataID = STATUSID;
    // status_packet.dataLength = DATALENGTH;
    // status_packet.data = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    // setChecksum(status_packet);
    uint8_t statusArray[14] ={0xA5,0x40,0x93,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80};
    //创建查询电池异常的数据帧
    // Packet error_packet;
    // error_packet.startFlag = STARTFLAG;
    // error_packet.address = UPPERADD;
    // error_packet.dataID = ERRORID;
    // error_packet.dataLength = DATALENGTH;
    // error_packet.data = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    // setChecksum(error_packet);
    uint8_t errorArray[14] ={0xA5,0x40,0x98,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x85};
    while(ros::ok())
    {
        // Packet rev_elect_packet;
        // Packet rev_status_packet;
        // Packet rev_error_packet; 
        //设置接收返回的数组
        uint8_t rev_electArray[14];
        uint8_t rev_statustArray[14];
        uint8_t rev_errorArray[14];    
        sp.write(electArray,sizeof(electArray));
        sp.read(rev_electArray,sizeof(rev_electArray));
        //读取电量
        uint16_t soc_raw = (rev_electArray[10] << 8) | rev_electArray[11];
        Battery.electricity = soc_raw * 0.1;
        //读取电池状态
        sp.write(statusArray,sizeof(statusArray));
        sp.read(rev_statustArray,sizeof(rev_statustArray));
        switch (rev_statustArray[4])
        {
        case 0:
            Battery.status = "静止状态";
            break;
        case 1:
            Battery.status = "充电状态";
            break;
        case 2:
            Battery.status = "放电状态";
            break;
        default:
            break;
        }
        battery_pub.publish(Battery);
        sp.write(errorArray,sizeof(errorArray));
        sp.read(rev_errorArray,sizeof(rev_errorArray));
        if(rev_errorArray[5] | rev_errorArray[6]|rev_errorArray[7]|rev_errorArray[8]|rev_errorArray[9]|rev_errorArray[10] | rev_errorArray[11])
        {
            std::stringstream battery_error_msg;
            battery_error_msg << "电池有异常";
            msg.data = battery_error_msg.str();
            battery_error.publish(msg);
        }
        else
        {
            std::stringstream battery_error_msg;
            battery_error_msg << "电池没有异常";
            msg.data = battery_error_msg.str();
        }
        loop_rate.sleep();
        ros::spinOnce();
    }
    return 0;
}