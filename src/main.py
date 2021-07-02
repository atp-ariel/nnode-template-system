from signal import SIGINT, signal
from .console.main import main
from .console.functions import verify_kill

if __name__ == "__main__":
    signal(SIGINT, verify_kill)
    main()
