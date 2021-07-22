//
// Created by peter on 2021/5/17.
//
#ifndef RM_MANUAL_REFEREE_H_
#define RM_MANUAL_REFEREE_H_

#include <cstdint>
#include <serial/serial.h>
#include <ros/ros.h>

#include <rm_common/referee/data.h>
#include <rm_msgs/Referee.h>
#include <rm_msgs/SuperCapacitor.h>
#include <rm_msgs/ChassisCmd.h>
#include <rm_msgs/GimbalCmd.h>
#include <rm_msgs/ShootCmd.h>

namespace rm_manual {
class SuperCapacitor {
 public:
  SuperCapacitor(rm_common::CapacityData &data) : data_(data) {};
  void read(const std::vector<uint8_t> &rx_buffer);
  ros::Time last_get_capacitor_data_ = ros::Time::now();
 private:
  void dtpReceivedCallBack(unsigned char receive_byte);
  void receiveCallBack(unsigned char package_id, const unsigned char *data);
  static float int16ToFloat(unsigned short data0);
  rm_common::CapacityData data_;
  unsigned char receive_buffer_[1024] = {0};
  unsigned char ping_pong_buffer_[1024] = {0};
  unsigned int receive_buf_counter_ = 0;
};

class Referee {
 public:
  Referee() : super_capacitor_(referee_data_.capacity_data) { referee_data_.robot_hurt_.hurt_type_ = 0x09; };
  void init();
  void read();
  void sendUi(const rm_common::GraphConfig &config, const std::string &content);
  void sendInteractiveData(int data_cmd_id, int receiver_id, unsigned char data);

  ros::Publisher referee_pub_;
  ros::Publisher super_capacitor_pub_;

  rm_msgs::Referee referee_pub_data_;
  rm_msgs::SuperCapacitor super_capacitor_pub_data_;
  rm_common::RefereeData referee_data_{};
  SuperCapacitor super_capacitor_;
 private:
  int unpack(uint8_t *rx_data);
  void pack(uint8_t *tx_buffer, uint8_t *data, int cmd_id, int len) const;
  void getRobotInfo();
  void publishData();

  serial::Serial serial_;
  ros::Time last_get_referee_data_ = ros::Time::now();
  const std::string serial_port_ = "/dev/usbReferee";
  const int k_frame_length_ = 128, k_header_length_ = 5, k_cmd_id_length_ = 2, k_tail_length_ = 2;
  static const int k_unpack_buffer_length_ = 256;
  uint8_t unpack_buffer_[k_unpack_buffer_length_]{};
  int client_id_ = 0;
};

// CRC verification
uint8_t getCRC8CheckSum(unsigned char *pch_message, unsigned int dw_length, unsigned char uc_crc_8);
uint32_t verifyCRC8CheckSum(unsigned char *pch_message, unsigned int dw_length);
void appendCRC8CheckSum(unsigned char *pch_message, unsigned int dw_length);
uint16_t getCRC16CheckSum(uint8_t *pch_message, uint32_t dw_length, uint16_t w_crc);
uint32_t verifyCRC16CheckSum(uint8_t *pch_message, uint32_t dw_length);
void appendCRC16CheckSum(unsigned char *pch_message, unsigned int dw_length);
}

#endif //RM_MANUAL_REFEREE_H_