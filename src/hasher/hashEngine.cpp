#include "hashEngine.hpp"

#include <openssl/evp.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../utility/getFilePath.hpp"
std::string HashEngine::hashFile(eventData& data)
{
    //
    std::cout << "Hashing started\n";
    eventData copyData = data;
    std::filesystem::path fullPath = getFullPath(copyData);
    std::ifstream ifs(fullPath, std::ios::binary);
    if (!ifs)
    {
        std::cerr << std::string("Error opening file for hashing: " + fullPath.string()) << "\n";
    }
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        keepHashing_ = false;
        std::cerr << "EVP_MD_CTX_new failed\n";
        return "";
    }

    int error = EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    if (error != 1)
    {
        keepHashing_ = false;
        std::cerr << "EVP_DigestInit_ex failed" << std::endl;
        return "";
    }
    char buffer[65536];
    while (ifs.read(buffer, sizeof(buffer)) || (ifs.gcount() > 0))
    {
        int status = EVP_DigestUpdate(ctx, buffer, static_cast<size_t>(ifs.gcount()));
        if (status != 1)
        {
            keepHashing_ = false;
            std::cerr << "EVP_DigestUpdate failed" << std::endl;
            return "";
        }
    }
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    int status = EVP_DigestFinal_ex(ctx, hash, &hash_len);
    if (status != 1)
    {
        keepHashing_ = false;
        std::cerr << "EVP_DigestFinal_ex failed" << std::endl;
        return "";
    }
    EVP_MD_CTX_free(ctx);
    std::ostringstream oss;
    for (unsigned int i = 0; i < hash_len; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    std::cout << oss.str() << std::endl;
    return std::string(oss.str());
}

void HashEngine::populateOutgoingQueue(eventData& data) { outgoingQueueHandler_->pushData(data); }

void HashEngine::cleanUpHashEngine()
{
    while (!hashQueueHandler_->isQueueEmpty())
    {
        hashQueueHandler_->waitReturnPopData();
    }
    keepHashing_ = false;
}
