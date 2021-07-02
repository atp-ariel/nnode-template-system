from src.console.functions import print_welcome, print_prompt, command_detection
from src.backend.proxy_backend import get_template_class
def main():
    print_welcome()
    while True:
        print_prompt()
        line = input()
        command_detection(line)
