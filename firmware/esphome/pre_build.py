Import("env")  # noqa: F821
import os
import shutil

project_dir = env.subst("$PROJECT_DIR")
build_dir = env.subst("$BUILD_DIR")

# Copy www folder to build directory
www_src = os.path.join(project_dir, "www")
www_dst = os.path.join(build_dir, "www")

if os.path.exists(www_src):
    if os.path.exists(www_dst):
        shutil.rmtree(www_dst)
    shutil.copytree(www_src, www_dst)
    print(f"Copied www folder to {www_dst}")
else:
    print("No www folder found in project directory")
