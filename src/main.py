from signal import SIGINT, signal
from functions import verify_kill
from functions import print_welcome, print_prompt, command_detection

def main():
    print_welcome()
    while True:
        print_prompt()
        line = input()
        command_detection(line)

if __name__ == "__main__":
    signal(SIGINT, verify_kill)
    main()
