# UART Bruteforcing
A universal asynchronous receiver transmitter (UART) is a computer hardware standard for asynchronous serial communication in which data is encoded in a serial format and transmitted at configurable speed. The following tools are aimed at the hardware Security Tester and a written in C. 'sbrute' is login brute-forcing tool for UART. Most UART connected are used to login to an embedded system. This tool is designed to allow a user to brute force the UART authentication process
```
$ ./sbrute -h
|> Serial Bruteforce v1.0
|> github.com/Er3X
|> A C-based tool for serial bruteforce attacks

USAGE:
	sbrute [OPTIONS]

OPTIONS:
	-h	Show help message and exit

	-b	The baud rate to be used [default: 115200]

	-d	The serial device to connect to [default: /dev/ttyUSB0]

	-f	The password file to be used [default: pass.txt]

	-l	String that defines the username login prompt [default: Login:]

	-p	String that defines the login password prompt [default: Password:]

	-s	String that defines a successful login [default: root@localhost$]

	-t	Timeout value for the serial connection [default: 3]

	-u	The username to be used for brute forcing [default: root]

	-v	Show verbose for additional info and debugging

	-a	Use this option if you don't know the success string. After submitting the username and password, the tool will check whether the response contains the login prompt again. If it doesn't, the credentials are likely correct. However, some systems may display additional lines after entering a password—regardless of success or failure. In such cases, you must provide the -c option to specify how many lines to skip before evaluating the response

	-c	Lines to skip after login attempt [default: 0]

```

## Build
```
git clone https://github.com/Er3X/UART-Bruteforce.git
cd UART-Bruteforce/src
gcc main.c -o sbrute
./sbrute -h
```
## Basic Usage
```
./sbrute -d /dev/ttyUSB0 -b 115200 -u root -f passwords.txt -l Login: -p Password: -s root@localhost$
```
- `-d /dev/ttyUSB0` : Serial device to use

- `-b 115200`      : Baud rate for the connection

- `-u root`        : Username to try

- `-f passwords.txt`: File containing one password per line

- `-l Login:` : Expected login prompt (matches before sending username)

- `-p Password:` : Expected password prompt (matches before sending password)

- `-s root@localhost$` : String to identify a successful login

## Advanced Modes

- **Verbose (-v):**
Shows verbose output

- **Auto-detect (-a):**
If you don’t know what success looks like, use this. The tool will assume login succeeded if the username prompt does not reappear after sending credentials. If the device prints extra lines on both success and failure, add -c N to skip those
```
./sbrute -a -c 2 -u admin -f passwords.txt
```

Here it will skip 2 lines after submitting credentials before checking if Login: is back

## Troubleshooting

- Stuck waiting for prompt:

  - Verify the exact prompt text (maybe lacks a newline). Use -l or -p to set it correctly

- Permission denied opening /dev/ttyUSB0:

  - Add your user to the dialout or tty group, or run with sudo

- No successful login found:

  - Check your wordlist for correct passwords

  - Adjust success string with -s or try auto mode

- Something else?
  - Feel free to open an issue on the GitHub repository


## Contribution

Contributions to UART-Bruteforce are welcome. If you encounter issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## Disclaimer

This tool is created for educational and testing purposes only. The authors and contributors are not responsible for any illegal, unethical, or unauthorized use of this tool. Users are solely responsible for ensuring that their use of this tool complies with all applicable laws, regulations, and ethical standards.

Usage of this tool on systems or networks without explicit authorization is strictly prohibited. The authors and contributors disclaim any responsibility for any damage, loss of data, or other consequences resulting from the use of this tool.

By using this tool, you acknowledge that you have read, understood, and agree to abide by the terms of this disclaimer.

**Use responsibly and ethically.**

## License
This project is licensed under the [MIT License.](https://github.com/Er3X/UART-Bruteforce/blob/main/LICENSE)
