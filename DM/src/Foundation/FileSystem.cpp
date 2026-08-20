#include <Foundation/FileSystem.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdint>


namespace DM
{
    namespace fs = std::filesystem;
    using namespace std::chrono;

    // ============ 文件读写 ============

    bool FileSystem::ReadFile(const std::string& path, std::string& outContent)
    {
        try
        {
            std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
            if (!file.is_open())
                return false;

            std::streamsize size = file.tellg();
            if (size <= 0)
            {
                outContent.clear();
                return true;
            }

            file.seekg(0, std::ios::beg);
            outContent.resize(static_cast<size_t>(size));

            if (!file.read(outContent.data(), size))
                return false;

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::ReadFileBinary(const std::string& path, std::vector<uint8_t>& outContent)
    {
        try
        {
            std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
            if (!file.is_open())
                return false;

            std::streamsize size = file.tellg();
            if (size <= 0)
            {
                outContent.clear();
                return true;
            }

            file.seekg(0, std::ios::beg);
            outContent.resize(static_cast<size_t>(size));

            if (!file.read(reinterpret_cast<char*>(outContent.data()), size))
                return false;

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::WriteFile(const std::string& path, const std::string& content)
    {
        try
        {
            fs::path filePath(path);
            if (filePath.has_parent_path())
                fs::create_directories(filePath.parent_path());

            std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!file.is_open())
                return false;

            file.write(content.data(), content.size());
            return file.good();
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::WriteFileBinary(const std::string& path, const std::vector<uint8_t>& content)
    {
        try
        {
            fs::path filePath(path);
            if (filePath.has_parent_path())
                fs::create_directories(filePath.parent_path());

            std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!file.is_open())
                return false;

            file.write(reinterpret_cast<const char*>(content.data()), content.size());
            return file.good();
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::AppendFile(const std::string& path, const std::string& content)
    {
        try
        {
            fs::path filePath(path);
            if (filePath.has_parent_path())
                fs::create_directories(filePath.parent_path());

            std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::app);
            if (!file.is_open())
                return false;

            file.write(content.data(), content.size());
            return file.good();
        }
        catch (...)
        {
            return false;
        }
    }

    // ============ 文件/目录信息 ============

    bool FileSystem::Exists(const std::string& path)
    {
        try
        {
            return fs::exists(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::IsRegularFile(const std::string& path)
    {
        try
        {
            return fs::is_regular_file(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::IsDirectory(const std::string& path)
    {
        try
        {
            return fs::is_directory(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::IsSymlink(const std::string& path)
    {
        try
        {
            return fs::is_symlink(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::IsEmpty(const std::string& path)
    {
        try
        {
            if (!fs::exists(path))
                return false;

            if (fs::is_directory(path))
                return fs::is_empty(path);
            else if (fs::is_regular_file(path))
                return fs::file_size(path) == 0;

            return false;
        }
        catch (...)
        {
            return false;
        }
    }

    uint64_t FileSystem::GetFileSize(const std::string& path)
    {
        try
        {
            if (!fs::exists(path) || !fs::is_regular_file(path))
                return 0;

            return static_cast<uint64_t>(fs::file_size(path));
        }
        catch (...)
        {
            return 0;
        }
    }

    uint64_t FileSystem::GetLastWriteTime(const std::string& path)
    {
        try
        {
            if (!fs::exists(path))
                return 0;

            auto ftime = fs::last_write_time(path);
            auto sctp = time_point_cast<system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + system_clock::now()
            );
            return duration_cast<seconds>(sctp.time_since_epoch()).count();
        }
        catch (...)
        {
            return 0;
        }
    }

    std::string FileSystem::GetLastWriteTimeString(const std::string& path, const std::string& format)
    {
        try
        {
            if (!fs::exists(path))
                return "";

            auto ftime = fs::last_write_time(path);
            auto sctp = time_point_cast<system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + system_clock::now()
            );

            auto tt = system_clock::to_time_t(sctp);
            std::tm tm;
#ifdef _WIN32
            localtime_s(&tm, &tt);
#else
            localtime_r(&tt, &tm);
#endif

            std::stringstream ss;
            ss << std::put_time(&tm, format.c_str());
            return ss.str();
        }
        catch (...)
        {
            return "";
        }
    }

    // ============ 路径操作 ============

    std::string FileSystem::GetParentPath(const std::string& path)
    {
        try
        {
            fs::path p(path);
            if (p.has_parent_path())
                return p.parent_path().string();
            return "";
        }
        catch (...)
        {
            return "";
        }
    }

    std::string FileSystem::GetFileName(const std::string& path)
    {
        try
        {
            return fs::path(path).filename().string();
        }
        catch (...)
        {
            return "";
        }
    }

    std::string FileSystem::GetFileNameWithoutExtension(const std::string& path)
    {
        try
        {
            return fs::path(path).stem().string();
        }
        catch (...)
        {
            return "";
        }
    }

    std::string FileSystem::GetExtension(const std::string& path)
    {
        try
        {
            return fs::path(path).extension().string();
        }
        catch (...)
        {
            return "";
        }
    }

    std::string FileSystem::GetExtensionWithoutDot(const std::string& path)
    {
        std::string ext = GetExtension(path);
        if (!ext.empty() && ext[0] == '.')
            return ext.substr(1);
        return ext;
    }

    void FileSystem::ChangeFileName(std::string& path, const std::string& newName)
    {
        try
        {
            fs::path p(path);
            if (!p.has_filename())
                return;

            fs::path parent = p.parent_path();
            std::string ext = p.extension().string();

            // 如果新名称不含扩展名，保留原扩展名
            fs::path newNamePath(newName);
            if (newNamePath.extension().empty() && !ext.empty())
            {
                fs::path newPath = parent / (newName + ext);
                path = newPath.string();
            }
            else
            {
                fs::path newPath = parent / newName;
                path = newPath.string();
            }
        }
        catch (...)
        {
        }
    }

    void FileSystem::AppendStrAfterFileName(std::string& path, const std::string& str)
    {
        try
        {
            fs::path p(path);
            if (!p.has_filename())
                return;

            std::string stem = p.stem().string();
            std::string ext = p.extension().string();
            std::string newFilename = stem + str + ext;

            fs::path parent = p.parent_path();
            fs::path newPath = parent / newFilename;
            path = newPath.string();
        }
        catch (...)
        {
        }
    }

    void FileSystem::AppendStrAfterExtension(std::string& path, const std::string& str)
    {
        try
        {
            fs::path p(path);
            if (!p.has_filename())
                return;

            std::string fullName = p.filename().string();
            std::string newFullName = fullName + str;

            fs::path parent = p.parent_path();
            fs::path newPath = parent / newFullName;
            path = newPath.string();
        }
        catch (...)
        {
        }
    }

    std::string FileSystem::ReplaceExtension(const std::string& path, const std::string& newExtension)
    {
        try
        {
            fs::path p(path);
            if (!p.has_filename())
                return path;

            std::string stem = p.stem().string();
            std::string ext = newExtension;
            if (!ext.empty() && ext[0] != '.')
                ext = "." + ext;

            fs::path parent = p.parent_path();
            fs::path newPath = parent / (stem + ext);
            return newPath.string();
        }
        catch (...)
        {
            return path;
        }
    }

    std::string FileSystem::JoinPath(const std::string& base, const std::string& relative)
    {
        try
        {
            fs::path p(base);
            p /= relative;
            return p.string();
        }
        catch (...)
        {
            return base + "/" + relative;
        }
    }

    std::string FileSystem::NormalizePath(const std::string& path)
    {
        try
        {
            std::string result = path;
            std::replace(result.begin(), result.end(), '\\', '/');

            // 移除末尾分隔符(除非是根目录)
            while (result.size() > 1 && result.back() == '/')
                result.pop_back();

            return result;
        }
        catch (...)
        {
            return path;
        }
    }

    std::string FileSystem::GetAbsolutePath(const std::string& path)
    {
        try
        {
            return fs::absolute(path).string();
        }
        catch (...)
        {
            return path;
        }
    }

    std::string FileSystem::Canonicalize(const std::string& path)
    {
        try
        {
            return fs::canonical(path).string();
        }
        catch (...)
        {
            return path;
        }
    }

    bool FileSystem::IsAbsolutePath(const std::string& path)
    {
        try
        {
            return fs::path(path).is_absolute();
        }
        catch (...)
        {
            return false;
        }
    }

    std::string FileSystem::GetRelativePath(const std::string& path, const std::string& base)
    {
        try
        {
            return fs::relative(path, base).string();
        }
        catch (...)
        {
            return path;
        }
    }

    // ============ 目录操作 ============

    std::vector<std::string> FileSystem::ListFiles(const std::string& directory, bool bRecursive)
    {
        std::vector<std::string> result;
        try
        {
            if (!fs::exists(directory) || !fs::is_directory(directory))
                return result;

            if (bRecursive)
            {
                for (const auto& entry : fs::recursive_directory_iterator(directory))
                {
                    if (fs::is_regular_file(entry.path()))
                        result.push_back(entry.path().string());
                }
            }
            else
            {
                for (const auto& entry : fs::directory_iterator(directory))
                {
                    if (fs::is_regular_file(entry.path()))
                        result.push_back(entry.path().string());
                }
            }
        }
        catch (...)
        {
            result.clear();
        }
        return result;
    }

    std::vector<std::string> FileSystem::ListEntries(const std::string& directory, bool bRecursive)
    {
        std::vector<std::string> result;
        try
        {
            if (!fs::exists(directory) || !fs::is_directory(directory))
                return result;

            if (bRecursive)
            {
                for (const auto& entry : fs::recursive_directory_iterator(directory))
                    result.push_back(entry.path().string());
            }
            else
            {
                for (const auto& entry : fs::directory_iterator(directory))
                    result.push_back(entry.path().string());
            }
        }
        catch (...)
        {
            result.clear();
        }
        return result;
    }

    std::vector<std::string> FileSystem::ListDirectories(const std::string& directory, bool bRecursive)
    {
        std::vector<std::string> result;
        try
        {
            if (!fs::exists(directory) || !fs::is_directory(directory))
                return result;

            if (bRecursive)
            {
                for (const auto& entry : fs::recursive_directory_iterator(directory))
                {
                    if (fs::is_directory(entry.path()))
                        result.push_back(entry.path().string());
                }
            }
            else
            {
                for (const auto& entry : fs::directory_iterator(directory))
                {
                    if (fs::is_directory(entry.path()))
                        result.push_back(entry.path().string());
                }
            }
        }
        catch (...)
        {
            result.clear();
        }
        return result;
    }

    std::vector<std::string> FileSystem::ListFilesWithPattern(const std::string& directory, const std::string& pattern, bool bRecursive)
    {
        std::vector<std::string> result;
        try
        {
            if (!fs::exists(directory) || !fs::is_directory(directory))
                return result;

            auto files = ListFiles(directory, bRecursive);
            for (const auto& file : files)
            {
                if (GetFileName(file).find(pattern) != std::string::npos)
                    result.push_back(file);
            }
        }
        catch (...)
        {
            result.clear();
        }
        return result;
    }

    bool FileSystem::CreateDirectories(const std::string& path)
    {
        try
        {
            if (fs::exists(path))
                return fs::is_directory(path);

            return fs::create_directories(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::CreateDirectorySingle(const std::string& path)
    {
        try
        {
            if (fs::exists(path))
                return fs::is_directory(path);

            return fs::create_directory(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::DeleteDirectory(const std::string& path)
    {
        try
        {
            if (!fs::exists(path) || !fs::is_directory(path))
                return false;

            if (!fs::is_empty(path))
                return false;

            return fs::remove(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::DeleteDirectoryRecursive(const std::string& path)
    {
        try
        {
            if (!fs::exists(path) || !fs::is_directory(path))
                return false;

            return fs::remove_all(path) > 0;
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::ClearDirectory(const std::string& path)
    {
        try
        {
            if (!fs::exists(path) || !fs::is_directory(path))
                return false;

            for (const auto& entry : fs::directory_iterator(path))
                fs::remove_all(entry.path());

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::CopyDirectory(const std::string& source, const std::string& destination, bool bRecursive)
    {
        try
        {
            if (!fs::exists(source) || !fs::is_directory(source))
                return false;

            if (bRecursive)
            {
                fs::copy(source, destination, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            }
            else
            {
                for (const auto& entry : fs::directory_iterator(source))
                {
                    if (fs::is_regular_file(entry.path()))
                    {
                        fs::path dest = fs::path(destination) / entry.path().filename();
                        fs::copy(entry.path(), dest, fs::copy_options::overwrite_existing);
                    }
                }
            }
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    // ============ 文件操作 ============

    bool FileSystem::DeleteFile_(const std::string& path)
    {
        try
        {
            if (!fs::exists(path) || !fs::is_regular_file(path)) return false;

            return fs::remove(path);
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::CopyFile_(const std::string& source, const std::string& destination, bool bOverwrite)
    {
        try
        {
            if (!fs::exists(source) || !fs::is_regular_file(source))
                return false;

            if (fs::exists(destination) && !bOverwrite)
                return false;

            fs::path destPath(destination);
            if (destPath.has_parent_path())
                fs::create_directories(destPath.parent_path());

            fs::copy(source, destination, fs::copy_options::overwrite_existing);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::Move(const std::string& source, const std::string& destination)
    {
        try
        {
            if (!fs::exists(source))
                return false;

            fs::path destPath(destination);
            if (destPath.has_parent_path())
                fs::create_directories(destPath.parent_path());

            fs::rename(source, destination);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool FileSystem::Rename(const std::string& source, const std::string& destination)
    {
        return Move(source, destination);
    }

    // ============ 系统路径 ============

    std::string FileSystem::GetCurrentDirectory_()
    {
        try
        {
            return fs::current_path().string();
        }
        catch (...)
        {
            return "";
        }
    }

    bool FileSystem::SetCurrentDirectory_(const std::string& path)
    {
        try
        {
            fs::current_path(path);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    std::string FileSystem::GetTempDirectory()
    {
        try
        {
            return fs::temp_directory_path().string();
        }
        catch (...)
        {
#ifdef _WIN32
            char* temp = nullptr;
            size_t len = 0;
            _dupenv_s(&temp, &len, "TEMP");
            std::string result = temp ? temp : "C:/Temp";
            free(temp);
            return result;
#else
            return "/tmp";
#endif
        }
    }

    std::string FileSystem::GetTempFileName_(const std::string& prefix, const std::string& extension)
    {
        try
        {
            fs::path tempDir = fs::temp_directory_path();
            std::string filename = prefix.empty() ? "tmp" : prefix;
            filename += "_" + std::to_string(
                duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count()
            );

            if (!extension.empty())
            {
                if (extension[0] != '.')
                    filename += ".";
                filename += extension;
            }
            else
            {
                filename += ".tmp";
            }

            return (tempDir / filename).string();
        }
        catch (...)
        {
            return "";
        }
    }

    std::string FileSystem::GetHomeDirectory()
    {
        try
        {
#ifdef _WIN32
            char* home = nullptr;
            size_t len = 0;
            _dupenv_s(&home, &len, "USERPROFILE");
            if (home)
            {
                std::string result(home);
                free(home);
                return result;
            }
            return "";
#else
            const char* home = getenv("HOME");
            if (home)
                return home;

            struct passwd* pw = getpwuid(getuid());
            return pw ? pw->pw_dir : "";
#endif
        }
        catch (...)
        {
            return "";
        }
    }

    std::string FileSystem::GetExecutableDirectory()
    {
        try
        {
#ifdef _WIN32
            char buffer[MAX_PATH];
            GetModuleFileNameA(NULL, buffer, MAX_PATH);
            return fs::path(buffer).parent_path().string();
#else
            char buffer[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
            if (len != -1)
            {
                buffer[len] = '\0';
                return fs::path(buffer).parent_path().string();
            }
            return "";
#endif
        }
        catch (...)
        {
            return "";
        }
    }

    // ============ 其他实用功能 ============

    std::string FileSystem::GenerateUniqueFileName(const std::string& directory, const std::string& baseName, const std::string& extension)
    {
        std::string ext = extension;
        if (!ext.empty() && ext[0] != '.')ext = "." + ext;
            

        std::string base = baseName;
        if (base.empty())base = "file";
            

        std::string path = JoinPath(directory, base + ext);
        int counter = 0;

        while (Exists(path))
        {
            ++counter;
            path = JoinPath(directory, base + "_" + std::to_string(counter) + ext);
        }

        return path;
    }

    bool FileSystem::EnsureDirectoryExists(const std::string& path)
    {
        return CreateDirectories(path);
    }

    bool FileSystem::EnsureFileDirectoryExists(const std::string& filePath)
    {
        std::string parent = GetParentPath(filePath);
        if (parent.empty())
            return true;

        return CreateDirectories(parent);
    }

    std::string FileSystem::GetFileMD5(const std::string& path)
    {
       
        return "";
    }

    bool FileSystem::AreFilesEqual(const std::string& path1, const std::string& path2)
    {
        try
        {
            if (!fs::exists(path1) || !fs::exists(path2))
                return false;

            if (GetFileSize(path1) != GetFileSize(path2))
                return false;

            std::string content1, content2;
            if (!ReadFile(path1, content1) || !ReadFile(path2, content2))
                return false;

            return content1 == content2;
        }
        catch (...)
        {
            return false;
        }
    }
}