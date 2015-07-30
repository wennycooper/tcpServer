#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include "std_msgs/String.h"
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

ros::Publisher pub;

void doprocessing (int sock);
void function1();

int main(int argc, char *argv[])
{
   int sockfd, newsockfd, portno;
   socklen_t clilen;

   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n, pid;

   ros::init(argc, argv, "tcpServer");
   ros::NodeHandle nh;

   pub = nh.advertise<geometry_msgs::Twist>("/example3_robot/cmd_vel", 10);

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0)
      {
      perror("ERROR opening socket");
      exit(1);
      }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }
   
   /* Now start listening for the clients, here
   * process will go in sleep mode and will wait
   * for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
      
      
   doprocessing(newsockfd);

}


void doprocessing (int sock)
{
   int n;
   char buffer[256];
   bool dirty = false;
   
   int count = 0;
   geometry_msgs::Twist twist;
   double linear_ = 0.0;
   double angular_ = 0.0;

   ros::Rate loop_rate(10);

   while(ros::ok()) {
       bzero(buffer, 256);
       count ++;
       n = read(sock,buffer,255);
   
       if (n < 0) {
         perror("ERROR reading from socket");
         exit(1);
       }

       dirty = false;

       if (n > 0) {
           buffer[n] = '\0';
           printf("Msg from socket buffer: %s (%d bytes)\n",buffer, n);

           if (!strncmp(buffer, "u", 1)) {
               printf("matched u\n");
               linear_ = 1.0;
               angular_ = 0.0;
               dirty = true;
           }

           if (!strncmp(buffer, "d", 1)) {
               printf("matched d\n");
               linear_ = -1.0;
               angular_ = 0.0;
               dirty = true;
           }

           if (!strncmp(buffer, "l", 1)) {
               printf("matched l\n");
               linear_ = 0.0;
               angular_ = 1.0;
               dirty = true;
           }

           if (!strncmp(buffer, "r", 1)) {
               printf("matched r\n");
               linear_ = 0.0;
               angular_ = -1.0;
               dirty = true;
           }

           if (!strncmp(buffer, "s", 1)) {
               printf("matched s\n");
               angular_ = 0.0;
               linear_ = 0.0;
               dirty = true;
           }


           twist.linear.x = 1.0 * linear_;
           twist.angular.z = 1.0 * angular_;
           
           if (dirty == true) {
               pub.publish(twist);   
               dirty = false;
           }
       }

       ros::spinOnce();
       loop_rate.sleep();
   
   }
}
