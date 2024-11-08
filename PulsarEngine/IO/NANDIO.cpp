#include <kamek.hpp>
#include <IO/NANDIO.hpp>
#include <Debug/Debug.hpp>
#include <IO/Logger.hpp>

namespace Pulsar {

//Virtual Funcs
bool NANDIO::CreateAndOpen(const char* path, u32 mode) {
    this->GetCorrectPath(this->filePath, path);
    
    // Open in regular write mode but seek to the end if we're in append mode
    if (mode == FILE_MODE_APPEND) {
        ISFS::CreateFile(this->filePath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
        if (this->OpenFileDirectly(this->filePath, FILE_MODE_WRITE)) {
            IOS::Seek(this->fd, 0, IOS::SEEK_END);  // Move to end for appending
            return true;
        }
    } else {
        return this->OpenFileDirectly(this->filePath, mode);
    }
    
    return false;
}


bool NANDIO::OpenFile(const char* path, u32 mode) {
    this->GetCorrectPath(this->filePath, path);
    if (!OpenFileDirectly(this->filePath, mode)) {
        char errorMessage[256];
        snprintf(errorMessage, sizeof(errorMessage), "NANDIO::OpenFile - Failed to open file at path, this does not mean it caused a crash, it just means its not found '%s'.", this->filePath);
        Logger::GetInstance().LogError(errorMessage);
        return false;
    }
    
    return true;
}

void NANDIO::GetCorrectPath(char* realPath, const char* path) const {
    int requiredLength = snprintf(realPath, IOS::ipcMaxPath, "%s%s", "/shared2/Pulsar", path);

    if (requiredLength < 0 || requiredLength >= IOS::ipcMaxPath) {
        char errorMessage[256];
        snprintf(errorMessage, sizeof(errorMessage),
                 "NANDIO::GetCorrectPath - Path exceeds maximum length: '%s%s'.", 
                 "/shared2/Pulsar", path);
        Debug::FatalError(errorMessage);
    }
}

//FOLDER
//Virtual funcs
bool NANDIO::FolderExists(const char* path) const {
    char realPath[IOS::ipcMaxPath];
    this->GetCorrectPath(realPath, path);
    u32 count;
    return ISFS::ReadDir(realPath, nullptr, &count) >= 0;
}

bool NANDIO::CreateFolder(const char* path) {
    if (type != IOType_ISO) {
        this->Bind(path);
        char realPath[IOS::ipcMaxPath];
        this->GetCorrectPath(realPath, path);

        s32 error = ISFS::CreateDir(realPath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
        
        // Check if folder creation was successful or if the folder already exists
        if (error >= 0 || error == ISFS::ERROR_FILE_EXISTS) {
            return true;
        }

        // Handle specific error case with Debug::FatalError
        char errorMessage[512];
        snprintf(errorMessage, sizeof(errorMessage), 
                 "NANDIO::CreateFolder - Failed to create folder at path '%s'. Error code: %d", 
                 realPath, error);
        Debug::FatalError(errorMessage);
    }

    // Return false if the type is IOType_ISO or if an error occurred
    return false;
}
void NANDIO::ReadFolder(const char* path) {
    this->Bind(path);
    char realPath[IOS::ipcMaxPath];
    this->GetCorrectPath(realPath, path);

    u32 count = maxFileCount;
    char* tmpArray = new(this->heap, 0x20) char[255 * (count + 1)];
    void* originalPtr = tmpArray;
    s32 error = ISFS::ReadDir(realPath, tmpArray, &count);
    if(error >= 0 && !isBusy) {
        isBusy = true;
        strncpy(this->folderName, path, IOS::ipcMaxPath);
        IOS::IPCPath* namesArray = new(this->heap, 0x20) IOS::IPCPath[count];
        u32 realCount = 0;
        char curFile[IOS::ipcMaxPath];
        while(tmpArray[0] != '\0') {
            u32 length = strlen(tmpArray);
            if(length > 255) break;
            if(length <= IOS::ipcMaxFileName) {
                snprintf(curFile, IOS::ipcMaxPath, "%s/%s", realPath, tmpArray);
                s32 curFilefd = ISFS::Open(curFile, ISFS::MODE_NONE);
                if(curFilefd >= 0) {
                    strcpy(namesArray[realCount], tmpArray);
                    ++realCount;
                    ISFS::Close(curFilefd);
                }
            }
            tmpArray = tmpArray + length + 1;
        }
        this->fileCount = realCount;
        this->fileNames = namesArray;
        this->isBusy = false;
    }
    EGG::Heap::free(originalPtr, this->heap);
}

bool NANDIO::RenameFile(const char* oldPath, const char* newPath) const {
    char realOldPath[IOS::ipcMaxPath];
    char realNewPath[IOS::ipcMaxPath];
    this->GetCorrectPath(realOldPath, oldPath);
    this->GetCorrectPath(realNewPath, newPath);
    return ISFS::Rename(realOldPath, realNewPath) >= 0;
}

}//namespace Pulsar