#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

ros::Publisher chatter_pub;

void doprocessing (int sock);

int main(int argc, char *argv[])
{
   int sockfd, newsockfd, portno;
   socklen_t clilen;

   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n, pid;

   ros::init(argc, argv, "tcpServer");
   ros::NodeHandle nh;

   chatter_pub = nh.advertise<std_msgs::String>("chatter", 1000);

   sleep(10); // to let me have enough time to run "rostopic echo chatter"

   std_msgs::String msg;
   std::stringstream ss;
   ss << "publish string on main()";
   msg.data = ss.str();

   chatter_pub.publish(msg);   // this works fine

   
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
   
   while (1)
   {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0)
         {
         perror("ERROR on accept");
         exit(1);
         }
      
      /* Create child process */
      pid = fork();
      if (pid < 0)
         {
         perror("ERROR on fork");
         exit(1);
         }
      
      if (pid == 0)
         {
         /* This is the client process */
         close(sockfd);
         doprocessing(newsockfd);
         exit(0);
         }
      else
         {
         close(newsockfd);
         }
   } /* end of while */
}


void doprocessing (int sock)
{
   int n;
   char buffer[256];
   
   bzero(buffer,256);
   
   int count = 0;
   while(ros::ok()) {
       bzero(buffer, 256);
       count ++;
       n = read(sock,buffer,255);
   
       if (n < 0) {
         perror("ERROR reading from socket");
         exit(1);
       }
   
       buffer[n] = '\0';
       printf("Here is the message: %s\n",buffer);
//       n = write(sock,"I got your message",18);

       std_msgs::String msg;
       std::stringstream ss;
       ss << buffer;
       msg.data = ss.str();

       ROS_INFO("I heard: [%s]", msg.data.c_str());

       chatter_pub.publish(msg);   // this just NOT work... :~

       ros::spinOnce();
   
//       if (n < 0) {
//         perror("ERROR writing to socket");
//         exit(1);
//       }
   }
}
