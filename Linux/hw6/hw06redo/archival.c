/*
 * archival.c
 * Colin Woods
 * Mar 14, 2021
 * COSC 3750
 * Homework 6
 *
 * A list of functions for creating and extracting archives
 *
 */

#include "wytar.h"


int processArg(const char* arg, FILE* archive) {
    struct stat entry;
    stat(arg, &entry);
    if ((entry.st_mode & S_IFMT) == S_IFREG) {
        // Process File
        if (writeHeader(createFile(entry, arg), archive)) {
            FILE* in = fopen(arg, "r");
            if (in == NULL) {
                perror(arg);
                return 0;
            }
            char buffer[512];
            memset(buffer, 0, 512);
            while (!feof(in)) {
                fread(buffer, sizeof(char), 512, in);
                fwrite(buffer, sizeof(char), 512, archive);
            }
            fclose(in);
            return 1;
        }
        return 0;
    } else if ((entry.st_mode & S_IFMT) == S_IFLNK) {
        // Process Link
        struct stat lnkStat;
        lstat(arg, &lnkStat);
        if (writeHeader(createLink(lnkStat, arg), archive)) {
            return 1;
        }
        return 0;
    } else if ((entry.st_mode & S_IFMT) == S_IFDIR) {
        // Process Directory
        if (writeHeader(createDir(entry, arg), archive)) {
            DIR* dir = opendir(arg);
            if (dir == NULL) {
                perror(arg);
                return 0;
            }
            struct dirent* d = readdir(dir);
            while (d != NULL) {
                if (!strcmp(".", d->d_name) || !strcmp("..", d->d_name)) {
                    d = readdir(dir);
                    continue;
                }
                char path[4096];
                strcpy(path, arg);
                strcat(path, "/");
                strcat(path, d->d_name);
                if (!processArg(path, archive)) {
                    return 0;
                }
                d = readdir(dir);
            }
            closedir(dir);
            return 1;
        }
    } else {
        // Bad file format
        printf(
            "Invalid file format.\n");
        return 0;
    }
    return 1;
}

struct tar_header createFile(struct stat file, const char *name)
{
    struct tar_header header;

    // Initialize block with 0s
    memset(header.name, 0, 100);
    memset(header.mode, 0, 8);
    memset(header.uid, 0, 8);
    memset(header.gid, 0, 8);
    memset(header.size, 0, 12);
    memset(header.mtime, 0, 12);
    memset(header.chksum, ' ', 8);
    memset(header.linkname, 0, 100);
    memset(header.magic, 0, 6);
    memset(header.version, 0, 2);
    memset(header.uname, 0, 32);
    memset(header.gname, 0, 32);
    memset(header.devmajor, 0, 8);
    memset(header.devminor, 0, 8);
    memset(header.prefix, 0, 155);
    memset(header.pad, 0, 12);

    // Name info
    int length = strlen(name);
    if (length < 100)
    {
        sprintf(header.name, "%s", name);
    }
    else
    {
        for (int i = 0; i < 99; i++)
        {
            header.name[i] = name[i];
        }
        header.name[99] = '\0';
        int i;
        for (i = 99; i < length; i++)
        {
            header.prefix[i - 99] = name[i];
        }
        header.prefix[i] = '\0';
    }

    // Mode info
    sprintf(header.mode, "%07o", file.st_mode);
    header.mode[1] = '0';

    // UID info
    sprintf(header.uid, "%07o", file.st_uid);

    // GID info
    sprintf(header.gid, "%07o", file.st_gid);

    // Size info
    unsigned int s = file.st_size;
    sprintf(header.size, "%011o", s);

    // Time info
    unsigned int t = file.st_mtim.tv_sec;
    sprintf(header.mtime, "%011o", t);

    // Set typeflag for file
    header.typeflag = REGTYPE;

    // Set magic
    for (int i = 0; i < TMAGLEN; i++)
    {
        header.magic[i] = TMAGIC[i];
    }
    header.magic[5] = ' ';

    // Set Version
    header.version[0] = ' ';
    header.version[1] = 0;

    // Set uname
    struct passwd *password = getpwuid(file.st_uid);
    sprintf(header.uname, "%s", password->pw_name);

    // Set gname
    struct group *grp = getgrgid(file.st_gid);
    sprintf(header.gname, "%s", grp->gr_name);

    // Calculate chksum
    unsigned int chk = 0;
    char *p = header.name;
    for (int i = 0; i < 512; i++)
    {
        chk += *p;
        p++;
    }

    // Set chksum
    sprintf(header.chksum, "%06o", chk);

    return header;
}

struct tar_header createLink(struct stat file, const char *name)
{
    struct tar_header header;

    // Initialize
    memset(header.name, 0, 100);
    memset(header.mode, 0, 8);
    memset(header.uid, 0, 8);
    memset(header.gid, 0, 8);
    memset(header.size, 0, 12);
    memset(header.mtime, 0, 12);
    memset(header.chksum, ' ', 8);
    memset(header.linkname, 0, 100);
    memset(header.magic, 0, 6);
    memset(header.version, 0, 2);
    memset(header.uname, 0, 32);
    memset(header.gname, 0, 32);
    memset(header.devmajor, 0, 8);
    memset(header.devminor, 0, 8);
    memset(header.prefix, 0, 155);
    memset(header.pad, 0, 12);

    // Name info
    int length = strlen(name);
    if (length < 100)
    {
        sprintf(header.name, "%s", name);
    }
    else
    {
        for (int i = 0; i < 99; i++)
        {
            header.name[i] = name[i];
        }
        header.name[99] = '\0';
        int i;
        for (i = 99; i < length; i++)
        {
            header.prefix[i - 99] = name[i];
        }
        header.prefix[i] = '\0';
    }

    // Mode info
    sprintf(header.mode, "%07o", file.st_mode);
    header.mode[1] = '0';

    // UID info
    sprintf(header.uid, "%07o", file.st_uid);

    // GID info
    sprintf(header.gid, "%07o", file.st_gid);

    // Size info
    unsigned int s = file.st_size;
    sprintf(header.size, "%011o", s);

    // Time info
    unsigned int t = file.st_mtim.tv_sec;
    sprintf(header.mtime, "%011o", t);

    // Set typeflag for file
    header.typeflag = SYMTYPE;

    // Set linkname
    readlink(name, header.linkname, 100);

    // Set magic
    for (int i = 0; i < TMAGLEN; i++)
    {
        header.magic[i] = TMAGIC[i];
    }
    header.magic[5] = ' ';

    // Set Version
    header.version[0] = ' ';
    header.version[1] = 0;

    // Set uname
    struct passwd *password = getpwuid(file.st_uid);
    sprintf(header.uname, "%s", password->pw_name);

    // Set gname
    struct group *grp = getgrgid(file.st_gid);
    sprintf(header.gname, "%s", grp->gr_name);

    // Calculate chksum
    unsigned int chk = 0;
    char *p = header.name;
    for (int i = 0; i < 512; i++)
    {
        chk += *p;
        p++;
    }

    // Set chksum
    sprintf(header.chksum, "%06o", chk);

    return header;
}

struct tar_header createDir(struct stat file, const char *name)
{
    struct tar_header header;

    // Initialize
    memset(header.name, 0, 100);
    memset(header.mode, 0, 8);
    memset(header.uid, 0, 8);
    memset(header.gid, 0, 8);
    memset(header.size, 0, 12);
    memset(header.mtime, 0, 12);
    memset(header.chksum, ' ', 8);
    memset(header.linkname, 0, 100);
    memset(header.magic, 0, 6);
    memset(header.version, 0, 2);
    memset(header.uname, 0, 32);
    memset(header.gname, 0, 32);
    memset(header.devmajor, 0, 8);
    memset(header.devminor, 0, 8);
    memset(header.prefix, 0, 155);
    memset(header.pad, 0, 12);

    // Name info
    int length = strlen(name);
    if (length < 100)
    {
        sprintf(header.name, "%s", name);
    }
    else
    {
        for (int i = 0; i < 99; i++)
        {
            header.name[i] = name[i];
        }
        header.name[99] = '\0';
        int i;
        for (i = 99; i < length; i++)
        {
            header.prefix[i - 99] = name[i];
        }
        header.prefix[i] = '\0';
    }

    // Mode info
    sprintf(header.mode, "%07o", file.st_mode);
    header.mode[1] = '0';

    // UID info
    sprintf(header.uid, "%07o", file.st_uid);

    // GID info
    sprintf(header.gid, "%07o", file.st_gid);

    // Size info
    unsigned int s = file.st_size;
    sprintf(header.size, "%011o", s);

    // Time info
    unsigned int t = file.st_mtim.tv_sec;
    sprintf(header.mtime, "%011o", t);

    // Set typeflag for file
    header.typeflag = DIRTYPE;

    // Set magic
    for (int i = 0; i < TMAGLEN; i++)
    {
        header.magic[i] = TMAGIC[i];
    }
    header.magic[5] = ' ';

    // Set Version
    header.version[0] = ' ';
    header.version[1] = 0;

    // Set uname
    struct passwd *password = getpwuid(file.st_uid);
    sprintf(header.uname, "%s", password->pw_name);

    // Set gname
    struct group *grp = getgrgid(file.st_gid);
    sprintf(header.gname, "%s", grp->gr_name);

    // Calculate chksum
    unsigned int chk = 0;
    char *p = header.name;
    for (int i = 0; i < 512; i++)
    {
        chk += *p;
        p++;
    }

    // Set chksum
    sprintf(header.chksum, "%06o", chk);

    return header;
}

int OctToDec(char *octString)
{

    // Converts octal string to decimal value
    unsigned int octal = atoi(octString);
    int num = octal;
    int dec_value = 0;

    int base = 1;

    int temp = num;
    while (temp)
    {

        int last_digit = temp % 10;
        temp = temp / 10;

        dec_value += last_digit * base;

        base = base * 8;
    }

    return dec_value;
}

int writeHeader(struct tar_header header, FILE *archive)
{
    //writes the header information
    return fwrite(&header, sizeof(char), 512, archive);
}


int extractArchive(const char *archName)
{
    //Extracts the object depending on whether it is a file, link, or directory.
        
    FILE *archive = fopen(archName, "r");
    if (archive == NULL)
    {
        perror(archName);
        return 0;
    }

    int blank = 0;
    while (!feof(archive))
    {
        
        struct tar_header header;

        char buf[512];
        fread(buf, sizeof(char), 512, archive);
        int b = 1;
        for (int i = 0; i < 512; i++)
        {
            if (buf[i] != 0)
            {
                b = 0;
            }
        }

        if (b)
        {
            if (blank != 0)
            {
                break;
            }
            blank++;
        }

        for (int i = 0; i < 512; i++)
        {
            header.name[i] = buf[i];
        }

        if (header.typeflag == REGTYPE)
        {
            // Extract a file
            if (!extractFile(header, archive))
            {
                return 0;
            }
        }
        else if (header.typeflag == SYMTYPE)
        {
            // Extract a link
            if (!extractLink(header, archive))
            {
                return 0;
            }
        }
        else if (header.typeflag == DIRTYPE)
        {
            // Extract a directory
            if (!extractDir(header, archive))
            {
                return 0;
            }
        }
        else
        {
            return 1;
        }
    }
    fclose(archive);
    return 1;
}

int extractFile(struct tar_header header, FILE *archive)
{
    char name[255];
    strcpy(name, header.name);
    strcat(name, header.prefix);

    FILE *out = fopen(name, "w+");
    if (out == NULL)
    {
        perror(name);
        return 0;
    }
    unsigned int size = OctToDec(header.size);
    int r = size % 512, count = ((size - r) / 512);

    for (int i = 0; i < count; i++)
    {
        char buffer[512];
        fread(buffer, sizeof(char), 512, archive);
        fwrite(buffer, sizeof(char), 512, out);
    }

    char buffer[512];
    fread(buffer, sizeof(char), 512, archive);
    fwrite(buffer, sizeof(char), strlen(buffer), out);

    fclose(out);

    unsigned int mod = OctToDec(header.mode);
    chmod(name, mod);

    unsigned int gid = OctToDec(header.gid);
    unsigned int uid = OctToDec(header.uid);
    chown(name, uid, gid);

    return 1;
}

int extractLink(struct tar_header header, FILE *archive)
{
    //Extract the link information
    char name[255];
    strcpy(name, header.name);
    strcat(name, header.prefix);

    FILE *out = fopen(name, "w+");
    if (out == NULL)
    {
        perror(name);
        return 0;
    }

    fclose(out);

    unsigned int mod = OctToDec(header.mode);
    chmod(name, mod);

    unsigned int gid = OctToDec(header.gid);
    unsigned int uid = OctToDec(header.uid);
    chown(name, uid, gid);

    symlink(name, header.linkname);

    return 1;
}

int extractDir(struct tar_header header, FILE *archive)
{
    //Extract the directory information
    char name[255];

    if (header.prefix[0] != '\0')
    {
        strncat(name, header.prefix, 155);
    }
    if (header.name[99] != '\0')
    {
        strncat(name, header.name, 100);
    }
    else
    {
        strcat(name, header.name);
    }

    unsigned int mod = OctToDec(header.mode);
    if (mkdir(header.name, mod) == -1)
    {
        return 0;
    }

    unsigned int gid = OctToDec(header.gid);
    unsigned int uid = OctToDec(header.uid);
    chown(name, uid, gid);
    return 1;
}

