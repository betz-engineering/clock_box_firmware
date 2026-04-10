Import("env")
import os

libdeps_dir = env.subst("$PROJECT_LIBDEPS_DIR/$PIOENV")
u8g2_path = os.path.join(libdeps_dir, "u8g2")
csrc_path = os.path.join(u8g2_path, "csrc")
src_path = os.path.join(u8g2_path, "src")

if os.path.exists(csrc_path):
    print(">>> Renaming 'csrc' to 'src' to isolate core library files...")
    os.rename(csrc_path, src_path)
