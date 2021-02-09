#include <cstdlib>
#include <ros/init.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <tuple>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <geometry_msgs/TransformStamped.h>

geometry_msgs::Pose to_geometry_msgs_pose(geometry_msgs::Transform const &transform) {
    geometry_msgs::Pose output;
    output.position.x = transform.translation.x;
    output.position.y = transform.translation.y;
    output.position.z = transform.translation.z;
    output.orientation.w = transform.rotation.w;
    output.orientation.x = transform.rotation.x;
    output.orientation.y = transform.rotation.y;
    output.orientation.z = transform.rotation.z;
    return output;
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "transform_stamped_to_odometry");

    ros::NodeHandle node_handle;
    ros::NodeHandle p_node_handle("~");

    std::string const transform_stamped_topic =
            p_node_handle.param("transform_stamped_topic", std::string(""));

    ros::Publisher odometry_pub(
                node_handle.advertise<nav_msgs::Odometry>("/odom", 5));

    int64_t counter = 0;
    ros::Subscriber sub_transform_stamped =
            node_handle.subscribe<geometry_msgs::TransformStamped>(                     //!@attention 需要显示指明模板
                transform_stamped_topic, 10,
                [&](const geometry_msgs::TransformStampedConstPtr &msg){
        nav_msgs::Odometry odometry_msg;
        odometry_msg.header = msg->header;
        odometry_msg.child_frame_id = msg->child_frame_id;
        odometry_msg.pose.pose = to_geometry_msgs_pose(msg->transform);
        odometry_pub.publish(odometry_msg);

        counter++;
        std::cerr << "processing count: " << counter << "\r";
    });

    std::cout << "start transform_stamped_to_odometry..." << std::endl;
    ros::spin();

    return EXIT_SUCCESS;
}
