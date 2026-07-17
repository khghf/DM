import subprocess
import sys

def main():
    print()
    print("正在生成 VS2022 解决方案...")
    
    try:
        # 直接运行 CMake
        result = subprocess.run([
            "cmake",
            "-S", ".",
            "-B", ".",
            "-G", "Visual Studio 17 2022",
            "-A", "x64"
        ])
        
        if result.returncode == 0:
            print()
            print("=" * 40)
            print("  生成成功！")
            print("=" * 40)
        else:
            print()
            print("[失败] 解决方案生成出错！")
            
    except FileNotFoundError:
        print()
        print("[错误] 找不到 CMake")
    except Exception as e:
        print()
        print(f"[错误] {e}")
    
    print()
    input("按 Enter 键退出...")

if __name__ == "__main__":
    main()