import os, subprocess, sys

def run_command(command, cwd=None):
    # run the command and check the output
    process = subprocess.Popen(
        command, cwd=cwd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
    )
    for line in process.stdout:
        print(line, end="")
    process.wait()
    if process.returncode != 0:
        raise subprocess.CalledProcessError(process.returncode, command)

def install_libxml2(software_path):

    # Create an install directory
    # build_dir = os.path.join("libxml2", "build")
    build_path   = software_path + "/build"
    install_path = software_path + "/install"
    os.makedirs(build_path,   exist_ok=True)
    os.makedirs(install_path, exist_ok=True)

    # Configure the build with CMake
    print("Configuring the build with CMake...")
    cmake_command = (
        f"cmake {software_path} -B {build_path} -DCMAKE_INSTALL_PREFIX={install_path} -DBUILD_SHARED_LIBS=ON "
        f"-DLIBXML2_WITH_TESTS=OFF -DLIBXML2_WITH_PYTHON=OFF"
    )
    run_command(cmake_command, cwd=build_path)

    # Build the library
    print("Building libxml2...")
    run_command(f"cmake --build {build_path} --config Release", cwd=build_path)

    # Install the library
    print(f"Installing libxml2 to {install_path}...")
    run_command(f"cmake --install {build_path}", cwd=build_path)

    print("libxml2 has been successfully installed!")

if __name__ == "__main__":

    if len(sys.argv) > 1:
        install_path = os.path.abspath(sys.argv[1])
    else:
        print("You need to define an install path")
        exit(1)

    print(f"Installing libxml2 to: {install_path}")
    os.makedirs(install_path, exist_ok=True)

    try:
        install_libxml2(install_path)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)