#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define GREEN            "\033[0;32m"
#define YELLOW           "\033[0;33m"
#define CYAN             "\033[0;36m"
#define WHITE            "\033[0;37m"

void help();
int init(int, int);
int send(int, const char*);
int receive(int, char*, size_t, int);
int line_count(FILE *fp);

int main(int argc, char *argv[]) {
  int opt, size, fd = -1, v = 0, a = 0;
  long int baudrate = 115200, timeout = 3, c = 0;
  char *device = NULL, *password_file = NULL, *username_prompt = NULL, *password_prompt = NULL, *login_success_string = NULL, *username = NULL;
  FILE *fp = NULL;

  while((opt = getopt(argc, argv, "hb:d:f:l:p:s:t:u:vac:")) != -1) {
    switch(opt) {
      case 'h':
        help();
        return 0;
      case 'b':
        baudrate = strtol(optarg, NULL, 10); 
        break;
      case 'd':
        device = strdup(optarg);
        break;
      case 'f':
        password_file = strdup(optarg);
        break;
      case 'l':
        username_prompt = strdup(optarg);
        break;
      case 'p':
        password_prompt = strdup(optarg);
        break;
      case 's':
        login_success_string = strdup(optarg);
        break;    
      case 't':
        timeout = strtol(optarg, NULL, 10);
        break;
      case 'u':
        username = strdup(optarg);
        break;    
      case 'v':
        v = 1;
        break;
      case 'a':
        a = 1;
        break;
      case 'c':
        c = strtol(optarg, NULL, 10);
        break;
      case '?':
        printf("QUITTING!\n");
        return 0;
    }
  }
  
  if(!device) {
    device = strdup("/dev/ttyUSB0");
  }

  if(!password_file) {
    password_file = strdup("pass.txt");
  }

  if(!username_prompt) {
    username_prompt = strdup("Login:");
  }

  if(!password_prompt) {
    password_prompt = strdup("Password:");
  }

  if(!login_success_string) {
    login_success_string = strdup("root@localhost$");
  }

  if(!username) {
    username = strdup("root");
  }

  fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

  if(fd < 0) {
    printf("Failed to open %s\n", device);
    goto cleanup;
  }
  
  if(!isatty(fd)) {
    printf("%s is not a TTY device\n", device);
    goto cleanup;
  }

  if(init(fd, baudrate) != 0) {
    printf("Error while initializing\n");
    goto cleanup;
  }

  fp = fopen(password_file, "r");
  
  if(fp == NULL) {
    fp = fopen(password_file, "r");
    if (!fp) {
    printf("Failed to open %s\n", password_file);
    goto cleanup;
    }
  }
  
  int bytes, count = 0;
  char buffer[256], line[256];
  
  int total_line = line_count(fp);
  char temp[256], temp2[256];
  rewind(fp);

  strcpy(temp2, username);
  temp2[strlen(temp2)] = '\0';

  for(int i = 0; i < total_line; i++) {
    fgets(line, sizeof(line), fp);
A:
    while(!strstr(buffer, username_prompt)) {
      if(count == 10) {
        printf("Not able to receive Login prompt\n");
        goto cleanup;
      }

      send(fd, "test\n");
      receive(fd, buffer, sizeof(buffer), timeout);
      count++;
    }

    if(username[strlen(username) - 1] != '\n') {
      username = realloc(username, strlen(username) + 2);
      strcat(username, "\n");
    }

    send(fd, username);
    receive(fd, buffer, sizeof(buffer), timeout);

    if(strstr(buffer, password_prompt)) {
      send(fd, line);
    }
    else {
      if(v == 1) {
        printf("The Login prompt was received, but the Password prompt was not\n");
      }
      goto A;
    }

    strcpy(temp, line);
    temp[strcspn(line, "\n")] = 0;
    printf("Trying username \"%s\" and password \"%s\"\n", temp2, temp);
    
    if(c != 0) {
      for (int i = 0; i < c; i++) {
        printf("i is used here %d\n", i); 
        receive(fd, buffer, sizeof(buffer), timeout);
      }
    }

    receive(fd, buffer, sizeof(buffer), timeout);

    if(strstr(buffer, login_success_string)) {
      printf("Found the right password %s\n", line);
      goto cleanup;
    }
    else if(a == 1 && strstr(buffer, username_prompt) == 0) {
      printf("Found the right password %s\n", line);
      goto cleanup;
    }
  }

  printf("Unable to find the right password from \"%s\" wordlist\n", password_file);

cleanup:
  if(fp) {
    fclose(fp);
  }
  if(fd >= 0) {
    close(fd);
  }
  free(password_file);
  free(device);
  free(username_prompt);
  free(password_prompt);
  free(login_success_string);
  free(username);
}

void help() {
  printf(GREEN"|> Serial Bruteforce v1.0\n");
  printf(CYAN"|> github.com/Er3X\n");
  printf(YELLOW"|> A C-based tool for serial bruteforce attacks\n\n");
  printf(WHITE"USAGE:\n\tsbrute [OPTIONS]\n\n");
  printf("OPTIONS:\n");
  printf(WHITE"\t-h\tShow help message and exit\n\n");
  printf("\t-b\tThe baud rate to be used [default: 115200]\n\n");
  printf("\t-d\tThe serial device to connect to [default: /dev/ttyUSB0]\n\n");
  printf("\t-f\tThe password file to be used [default: pass.txt]\n\n");
  printf("\t-l\tString that defines the username login prompt [default: Login:]\n\n");
  printf("\t-p\tString that defines the login password prompt [default: Password:]\n\n");
  printf("\t-s\tString that defines a successful login [default: root@localhost$]\n\n");
  printf("\t-t\tTimeout value for the serial connextion [default: 3]\n\n");
  printf("\t-u\tThe username to be used for brute forcing [default: root]\n\n");
  printf("\t-v\tShow verbose for additional info and debugging\n\n");
  printf("\t-a\tUse this option if you don't know the success string. After submitting the username and password, the tool will check whether the response contains the login prompt again. If it doesn't, the credentials are likely correct. However, some systems may display additional lines after entering a password—regardless of success or failure. In such cases, you must provide the -c option to specify how many lines to skip before evaluating the response\n\n");
  printf("\t-c\tLines to skip after login attempt [default: 0]\n\n");
}

int init(int fd, int baudrate) {
  speed_t speed;
  switch (baudrate) {
    case 0:        speed = B0;        break;
    case 50:       speed = B50;       break;
    case 75:       speed = B75;       break;
    case 110:      speed = B110;      break;
    case 134:      speed = B134;      break;
    case 150:      speed = B150;      break;
    case 200:      speed = B200;      break;
    case 300:      speed = B300;      break;
    case 600:      speed = B600;      break;
    case 1200:     speed = B1200;     break;
    case 1800:     speed = B1800;     break;
    case 2400:     speed = B2400;     break;
    case 4800:     speed = B4800;     break;
    case 9600:     speed = B9600;     break;
    case 19200:    speed = B19200;    break;
    case 38400:    speed = B38400;    break;
    case 57600:    speed = B57600;    break;
    case 115200:   speed = B115200;   break;
    case 230400:   speed = B230400;   break;
    case 460800:   speed = B460800;   break;
    case 500000:   speed = B500000;   break;
    case 576000:   speed = B576000;   break;
    case 921600:   speed = B921600;   break;
    case 1000000:  speed = B1000000;  break;
    case 1152000:  speed = B1152000;  break;
    case 1500000:  speed = B1500000;  break;
    case 2000000:  speed = B2000000;  break;
#ifdef B76800
    /* SPARC-specific rates */
    case 76800:    speed = B76800;    break;
    case 153600:   speed = B153600;   break;
    case 307200:   speed = B307200;   break;
    case 614400:   speed = B614400;   break;
#endif
    /* Non-SPARC extended rates (should be defined on most architectures) */
    case 2500000:  speed = B2500000;  break;
    case 3000000:  speed = B3000000;  break;
    case 3500000:  speed = B3500000;  break;
    case 4000000:  speed = B4000000;  break;
    default:
      printf("Unsupported baud rate: %d\n", baudrate);
      return -1;
  }

  struct termios config;

  if(tcgetattr(fd, &config) < 0) {
    printf("Error while getting the current configuration of the serial interface\n");
    return -1;
  }

  cfsetispeed(&config, speed);
  cfsetospeed(&config, speed);

  config.c_cflag &= ~PARENB;
  config.c_cflag &= ~CSTOPB;
  config.c_cflag &= ~CSIZE;
  config.c_cflag |= CS8;

  config.c_cflag |= CREAD | CLOCAL;
  config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  config.c_iflag &= ~(IXON | IXOFF | IXANY);
  config.c_oflag &= ~OPOST;

  config.c_cc[VMIN] = 0;
  config.c_cc[VTIME] = 10;

  if(tcsetattr(fd, TCSANOW, &config) < 0) {
    printf("Error while setting the current configuration of the serial interface\n");
    return -1;
  }
  
  return 0;
}

int send(int fd, const char *data) {
  int send = write(fd, data, strlen(data));

  if(send < 0) {
    printf("Failed to send data\n");
    return -1;
  }

  return send;
}

int receive(int fd, char *buffer, size_t buffer_size, int timeout_sec) {
  fd_set fds_to_read;

  struct timeval timeout;

  FD_ZERO(&fds_to_read);
  FD_SET(fd, &fds_to_read);

  timeout.tv_sec = timeout_sec;
  timeout.tv_usec = 0;

  int ready = select(fd + 1, &fds_to_read, NULL, NULL, &timeout);

  if(ready == -1) {
    printf("Failed select()");
    return -1;
  }
  else if(ready == 0) {
    return 0;
  }

  int bytes = read(fd, buffer, buffer_size - 1);

  if(bytes < 0) {
    printf("Read failed");
    return -1;
  }

  buffer[bytes] = '\0';
  return bytes;
}

int line_count(FILE *fp) {
  int count = 0;
  char ch;

  while((ch = fgetc(fp)) != EOF) {
    if(ch == '\n') {
      count++;
    }
  }

  return count;
}
