#ifndef MANIFEST_MANIFEST_HPP
#define MANIFEST_MANIFEST_HPP

#include "deluxe_lib/core/core.hpp"

#include <filesystem>
#include <string>

class Manifest
{
    DL_DECLARE_CLASS(Manifest)

public:
    /// @brief read new manifest
    /// @param path path to manifest folder
    /// @param manifest_name name of manifest file
    void readManifest(const std::filesystem::path& path, const std::string& manifest_name = "manifest.json");

private:
    std::string m_manifest_names = "manifest.json";
    std::filesystem::path m_manifest_path;

};

#endif // MANIFEST_MANIFEST_HPP
