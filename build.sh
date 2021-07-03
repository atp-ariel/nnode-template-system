# Load requirements
py -m pip install -r requirements.txt --index-url http://nexus.prod.uci.cu/repository/pypi-proxy/simple/ --trusted-host nexus.prod.uci.cu;

# clean repository
if [ -d "bin" ]; then 
    rm -rf bin && mkdir bin
fi

pyinstaller src/main.py --workpath bin/build/ --distpath bin/dist/ --specpath bin/ -n "nnode-template-system"

./bin/dist/nnode-template-system/nnode-template-system.exe
