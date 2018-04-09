/***********************************************************************************************************/
/***********************************************************************************************************/
/***********************************************************************************************************/
/******************* this code is a copy of [https://github.com/BartekkPL/bvh-parser] ***********************/
/***********************************************************************************************************/
/***********************************************************************************************************/
/***********************************************************************************************************/


#include "../../include/bvh-parser/bvh-parser.h"


#include <ios>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

/** Indicate whether bvh parser allows multi hierarchy or not
  * Not fully tested
  */
#define MULTI_HIERARCHY 0

namespace {

const std::string kChannels = "CHANNELS";
const std::string kEnd = "End";
const std::string kEndSite = "End Site";
const std::string kFrame = "Frame";
const std::string kFrames = "Frames:";
const std::string kHierarchy = "HIERARCHY";
const std::string kJoint = "JOINT";
const std::string kMotion = "MOTION";
const std::string kOffset = "OFFSET";
const std::string kRoot = "ROOT";

const std::string kXpos = "Xposition";
const std::string kYpos = "Yposition";
const std::string kZpos = "Zposition";
const std::string kXrot = "Xrotation";
const std::string kYrot = "Yrotation";
const std::string kZrot = "Zrotation";

}

namespace bvh {

//##############################################################################
// Main parse function
//##############################################################################
int Bvh_parser::parse(const std::string& path, Bvh* bvh) {
  std::cout <<  "Parsing file : " << path << std::endl;;

  path_ = path;
  bvh_ = bvh;

  std::ifstream file;
  file.open(path_);

  if (file.is_open()) {
    std::string token;

#if MULTI_HIERARCHY == 1
    while (file.good()) {
#endif
      file >> token;
      if (token == kHierarchy) {
        int ret = parse_hierarchy(file);
        if (ret)
          return ret;
      } else {
          std::cout << "ERROR: Bad structure of .bvh file. " << kHierarchy
                   << " should be on the top of the file" << std::endl;
        return -1;
      }
#if MULTI_HIERARCHY == 1
    }
#endif
  } else {
      std::cout << "ERROR: Cannot open file to parse : " << path_;
    return -1;
  }

  std::cout << "INFO:Successfully parsed file" << std::endl;
  return 0;
}

//##############################################################################
// Function parsing hierarchy
//##############################################################################
int Bvh_parser::parse_hierarchy(std::ifstream& file) {
    std::cout << "Parsing hierarchy" << std::endl;

  std::string token;
  int ret;

  if (file.good()) {
    file >> token;

    //##########################################################################
    // Parsing joints
    //##########################################################################
    if (token == kRoot) {
      std::shared_ptr <Joint> rootJoint;
      ret = parse_joint(file, nullptr, rootJoint);

      if (ret)
        return ret;

      std::cout << "INFO: There is " << bvh_->num_channels() << " data channels in the"
                << " file" << std::endl;

      bvh_->set_root_joint(rootJoint);
    } else {
        std::cout << "ERROR: Bad structure of .bvh file. Expected " << kRoot
                 << ", but found \"" << token << "\"" << std::endl;
      return -1;
    }
  }

  if (file.good()) {
    file >> token;

    //##########################################################################
    // Parsing motion data
    //##########################################################################
    if (token == kMotion) {
      ret = parse_motion(file);

      if (ret)
        return ret;
    } else {
        std::cout << "ERROR: Bad structure of .bvh file. Expected " << kMotion
                 << ", but found \"" << token << "\"" << std::endl;
      return -1;
    }
  }
  return 0;
}

//##############################################################################
// Function parsing joint
//##############################################################################
int Bvh_parser::parse_joint(std::ifstream& file,
    std::shared_ptr <Joint> parent, std::shared_ptr <Joint>& parsed) {

    std::cout << "TRACE: Parsing joint" << std::endl;;

  std::shared_ptr<Joint> joint = std::make_shared<Joint>();
  joint->set_parent(parent);

  std::string name;
  file >> name;

  std::cout << "TRACE: Joint name : " << name << std::endl;

  joint->set_name(name);

  std::string token;
  std::vector <std::shared_ptr <Joint>> children;
  int ret;

  file >> token;  // Consuming '{'
  file >> token;

  //############################################################################
  // Offset parsing
  //############################################################################
  if (token == kOffset) {
    Joint::Offset offset;

    try {
      file >> offset.x >> offset.y >> offset.z;
    } catch (const std::ios_base::failure e) {
        std::cout << "ERROR: Failure while parsing offset" << std::endl;
      return -1;
    }

    joint->set_offset(offset);

    std::cout << "TRACE: Offset x: " << offset.x << ", y: " << offset.y << ", z: "
               << offset.z << std::endl;

  } else {
      std::cout << "ERROR: Bad structure of .bvh file. Expected " << kOffset << ", but "
               << "found \"" << token << "\"" << std::endl;

    return -1;
  }

  file >> token;

  //############################################################################
  // Channels parsing
  //############################################################################
  if (token == kChannels) {
    ret = parse_channel_order(file, joint);

    std::cout << "TRACE: Joint has " << joint->num_channels() << " data channels" << std::endl;

    if (ret)
      return ret;
  } else {
      std::cout << "ERROR: Bad structure of .bvh file. Expected " << kChannels
               << ", but found \"" << token << "\"" << std::endl;

    return -1;
  }

  file >> token;

  bvh_->add_joint(joint);

  //############################################################################
  // Children parsing
  //############################################################################

  while (file.good()) {
    //##########################################################################
    // Child joint parsing
    //##########################################################################
    if (token == kJoint) {
      std::shared_ptr <Joint> child;
      ret = parse_joint(file, joint, child);

      if (ret)
        return ret;

      children.push_back(child);

    //##########################################################################
    // Child joint parsing
    //##########################################################################
    } else if (token == kEnd) {
      file >> token >> token;  // Consuming "Site {"

      std::shared_ptr <Joint> tmp_joint = std::make_shared <Joint> ();

      tmp_joint->set_parent(joint);
      tmp_joint->set_name(kEndSite);
      children.push_back(tmp_joint);

      file >> token;

      //########################################################################
      // End site offset parsing
      //########################################################################
      if (token == kOffset) {
        Joint::Offset offset;

        try {
          file >> offset.x >> offset.y >> offset.z;
        } catch (const std::ios_base::failure e) {
            std::cout << "ERROR: Failure while parsing offset" << std::endl;
          return -1;
        }

        tmp_joint->set_offset(offset);

        std::cout << "TRACE: Joint name : EndSite" << std::endl;
        std::cout << "TRACE: Offset x: " << offset.x << ", y: " << offset.y << ", z: "
                   << offset.z << std::endl;

        file >> token;  // Consuming "}"

      } else {
          std::cout << "ERROR: Bad structure of .bvh file. Expected " << kOffset
                   << ", but found \"" << token << "\"" << std::endl;

        return -1;
      }

      bvh_->add_joint(tmp_joint);
    //##########################################################################
    // End joint parsing
    //##########################################################################
    } else if (token == "}") {
      joint->set_children(children);
      parsed = joint;
      return 0;
    }

    file >> token;
  }

  std::cout << "ERROR: Cannot parse joint, unexpected end of file. Last token : "
             << token << std::endl;
  return -1;
}

//##############################################################################
// Motion data parse function
//##############################################################################
int Bvh_parser::parse_motion(std::ifstream& file) {

    std::cout << "INFO:" << "Parsing motion";

  std::string token;
  file >> token;

  int frames_num;

  if (token == kFrames) {
    file >> frames_num;
    bvh_->set_num_frames(frames_num);
    std::cout << "INFO: Num of frames : " << frames_num << std::endl;
  } else {
      std::cout << "ERROR: Bad structure of .bvh file. Expected " << kFrames
               << ", but found \"" << token << "\"" << std::endl;

    return -1;
  }

  file >> token;

  double frame_time;

  if (token == kFrame) {
    file >> token;  // Consuming 'Time:'
    file >> frame_time;
    bvh_->set_frame_time(frame_time);
    std::cout << "INFO: Frame time : " << frame_time << std::endl;

    float number;
    for (int i = 0; i < frames_num; i++) {
      for (auto joint : bvh_->joints()) {
        std::vector <float> data;
        for (int j = 0; j < joint->num_channels(); j++) {
          file >> number;
          data.push_back(number);
        }
        joint->add_frame_motion_data(data);
      }
    }
  } else {
      std::cout << "ERROR: Bad structure of .bvh file. Expected " << kFrame
               << ", but found \"" << token << "\"" << std::endl;

    return -1;
  }

  return 0;
}

//##############################################################################
// Channels order parse function
//##############################################################################
int Bvh_parser::parse_channel_order(std::ifstream& file,
    std::shared_ptr <Joint> joint) {

    std::cout << "TRACE: Parse channel order" << std::endl;

  int num;
  file >> num;
  std::cout << "TRACE: Number of channels : " << num << std::endl;

  std::vector <Joint::Channel> channels;
  std::string token;

  for (int i = 0; i < num; i++) {
    file >> token;
    if (token == kXpos)
      channels.push_back(Joint::Channel::XPOSITION);
    else if (token == kYpos)
      channels.push_back(Joint::Channel::YPOSITION);
    else if (token == kZpos)
      channels.push_back(Joint::Channel::ZPOSITION);
    else if (token == kXrot)
      channels.push_back(Joint::Channel::XROTATION);
    else if (token == kYrot)
      channels.push_back(Joint::Channel::YROTATION);
    else if (token == kZrot)
      channels.push_back(Joint::Channel::ZROTATION);
    else {
        std::cout << "ERROR: Not valid channel!" << std::endl;
      return -1;
    }
  }

  joint->set_channels_order(channels);
  return 0;
}

//std::string Bvh_parser::vtos(const std::vector <float>& vector) {
  //std::ostringstream oss;

  //if (!vector.empty())
  //{
    //// Convert all but the last element to avoid a trailing ","
    //std::copy(vector.begin(), vector.end()-1,
        //std::ostream_iterator<float>(oss, ", "));

    //// Now add the last element with no delimiter
    //oss << vector.back();
  //}

  //return oss.str();
//}

} // namespace
