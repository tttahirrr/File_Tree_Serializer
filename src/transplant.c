#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
* You may modify this file and/or move the functions
* contained here to other source files (except for main.c) as you wish.
*
* IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
* you MAY NOT declare any arrays or allocate any storage with malloc().
* The purpose of this restriction is to force you to use pointers.
*
* Variables to hold the pathname of the current file or directory
* as well as other data have been pre-declared for you in global.h.
* You must use those variables, rather than declaring your own.
* IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
*
* IMPORTANT: You MAY NOT use floating point arithmetic or declare
* any "float" or "double" variables. IF YOU VIOLATE THIS RESTRICTION,
* YOU WILL GET A ZERO!
*/


int strlength(const char *str) {
    const char *pointer = str; // pointer to iterate thru string

    while (*pointer != '\0') {
        pointer++;
    }
    return (int)(pointer - str); // difference between end pointer & start pointer
}


int strcompare(const char *str1, const char *str2) {
    // iterate thru both strings
    while (*str1 != '\0' && *str2 != '\0') {
        // if characters differ, return the difference
        if (*str1 != *str2) {
            return (unsigned char)*str1 - (unsigned char)*str2;
        }
        // move to the next characters in both strings
        str1++;
        str2++;
    }
    // if both strings same up to the end of one of them, return the difference in null terminators
    return (unsigned char)*str1 - (unsigned char)*str2;
}


uint32_t to_big_endian_32(uint32_t little_endian_value) {
    return ((little_endian_value & 0x000000FF) << 24) |
           ((little_endian_value & 0x0000FF00) << 8)  |
           ((little_endian_value & 0x00FF0000) >> 8)  |
           ((little_endian_value & 0xFF000000) >> 24);
}


uint32_t from_big_endian_32(uint32_t big_endian_value) {
    // since the operation is symmetric, we can use the same function.
    return to_big_endian_32(big_endian_value);
}


uint64_t to_big_endian_64(uint64_t little_endian_value) {
    return ((little_endian_value & 0x00000000000000FF) << 56) |
           ((little_endian_value & 0x000000000000FF00) << 40) |
           ((little_endian_value & 0x0000000000FF0000) << 24) |
           ((little_endian_value & 0x00000000FF000000) << 8)  |
           ((little_endian_value & 0x000000FF00000000) >> 8)  |
           ((little_endian_value & 0x0000FF0000000000) >> 24) |
           ((little_endian_value & 0x00FF000000000000) >> 40) |
           ((little_endian_value & 0xFF00000000000000) >> 56);
}


uint64_t from_big_endian_64(uint64_t big_endian_value) {
    // since the operation is symmetric, we can use the same function.
    return to_big_endian_64(big_endian_value);
}


// returns the value found in the next 8 bytes read.
// makes sure those bytes are converted from big endian to little endian.
uint64_t get_8bytes_big2little() {
   uint64_t value = 0; // 64 bit value to hold the bytes we read
   int read_byte;


   // step 1: read 8 bytes from input & construct the big endian value
   for (int i = 0; i < 8; i++)
   {
       if ((read_byte = getchar()) == EOF)
       {
           fprintf(stderr, "error. unexpected EOF while reading byte %d \n", i);
           return -1;
       }
       value = (value << 8) | (read_byte & 0xFF); // shift left & append the new byte
   }

   // step 2: convert the value from big-endian to little endian
   //return from_big_endian_64(value);
   return value;
}


// returns the value found in the next 4 bytes read.
// makes sure those bytes are converted from big endian to little endian.
uint32_t get_4bytes_big2little() {
   uint32_t value = 0; // 32 bit value to hold the bytes we read
   int read_byte;


   // step 1: read 4 bytes from input & construct the big-endian value
   for (int i = 0; i < 4; i++)
   {
       if ((read_byte = getchar()) == EOF)
       {
           fprintf(stderr, "error. unexpected EOF while reading byte %d", i);
           return -1;
       }
       value = (value << 8) | (read_byte & 0xFF); // shift left & append the new byte
   }

   // step 2: Convert the value from big-endian to little-endian
   //return from_big_endian_32(value);
   return value;
}


// checks the magic bytes, then checks if the depths match, then returns the record type
int get_record_type(int depth) {

   // step 1: check if the magic bytes are valid
   int read_char;
   // check for the 0C
   if ((read_char = getchar()) == EOF) {
       fprintf(stderr, "error. EOF while reading first magic byte");
       return -1;
   }
   else if (read_char != 0x0C) {
       fprintf(stderr, "error. expected 0x0C, got 0x%x", read_char);
       return -1;
   }

   // check for the 0D
   if ((read_char = getchar()) == EOF) {
       fprintf(stderr, "error. EOF while reading second magic byte");
       return -1;
   }
   else if (read_char != 0x0D) {
       fprintf(stderr, "error. expected 0x0D, got 0x%x", read_char);
       return -1;
   }

   // check for the ED
   if ((read_char = getchar()) == EOF) {
       fprintf(stderr, "error. EOF while reading third magic byte");
       return -1;
   }
   else if (read_char != 0xED) {
       fprintf(stderr, "error. expected 0xED, got 0x%x", read_char);
       return -1;
   }

   // step 2: read the type of the record
   int record_type;
   if ((record_type = getchar()) == EOF) {
       fprintf(stderr, "error reading record type. reached EOF. \n");
       return -1;
   }

   // step 3: use get_4bytes_big2little to read & convert the depth (4 bytes)
   uint32_t read_depth = get_4bytes_big2little();
   if (read_depth == (uint32_t)-1) {
       fprintf(stderr, "error reading depth. \n");
       return -1;
   }

   // step 3.1: check if the converted depth matches the provided depth parameter
   if (read_depth != (uint32_t)depth) {
       fprintf(stderr, "error. expected depth: %d. got: %u.", depth, read_depth);
       return -1;
   }

   // step 4: return the type of the record
   return record_type;
}


// reads size, gets file size, then writes file
int create_file() {
   uint64_t record_size = get_8bytes_big2little(); // read the next 8 bytes

   if (record_size == -1) {
       fprintf(stderr, "error reading record size. \n");
       return -1;
   }
   uint64_t file_size = record_size - 16; // subtract the header bytes to get file size

   FILE *file = fopen(path_buf, "w");
   if (file == NULL) {
       fprintf(stderr, "error.  couldn't open file. \n");
       return -1;
   }
   else {
       int read_char;

       for (int file_index = 0; file_index < file_size; file_index++) {
           if ((read_char = getchar()) == -1) {
               fprintf(stderr, "error. while getting a byte for file data. \n");
               return -1;
           }
           if (fputc(read_char, file) == EOF) {
               fprintf(stderr, "error. unable to write char to file. got EOF. \n");
               return -1;
           }
       }
       if (fclose(file) == -1) {
           fprintf(stderr, "error. couldn't close file pointer. \n");
           return -1;
       }
   }
   return 0;
}


int path_init(char *name) {
    int len = 0; // variable to store the length of the path
    char *p = name; // pointer to iterate through the input path

    // calculate the length of the input path 'name'
    while (*p != '\0') { // loop until the null terminator
        len++;
        p++;

        // check if path length exceeds the buffer size
        if (len >= PATH_MAX) {
            return -1; // path is too long to fit in path_buf
        }
    }

    // copy the path into path_buf character by character
    p = name; // reset pointer to the beginning of 'name'
    for (int i = 0; i < len; i++) {
        *(path_buf + i) = *(p + i); // copy each character into path_buf
    }
    *(path_buf + len) = '\0'; // null-terminate the path in path_buf

    // set the path_length global variable
    path_length = len;

    // return success
    return 0;
}


int path_push(char *name) {
    int name_len = 0; // to store the length of the new component
    char *p = name;   // pointer to iterate over the new component

    // step 1: calculate the length of the component 'name'
    while (*p != '\0') { // loop until the null terminator
        if (*p == '/') { // invalid input, contains a '/'
            return -1;
        }
        name_len++;
        p++;
    }

    // step 2: check if the new component will fit in path_buf
    // we need to add 1 for the '/' separator. if needed.
    if (*(path_buf + path_length - 1) != '/') {
        // if we need to add a '/', ensure the new component fits
        if (path_length + name_len + 1 >= PATH_MAX) {
            return -1; // not enough space with the added '/'
        }
    } else {
        // if no '/' is needed, check if just appending the component will fit
        if (path_length + name_len >= PATH_MAX) {
            return -1; // not enough space with the pre-existing '/'
        }
    }

    // step 3: add the '/' separator if the current path doesn't end with one
    if (*(path_buf + path_length - 1) != '/') {
        *(path_buf + path_length) = '/';
        path_length++;
    }

    // step 4: append the new component 'name' to path_buf
    p = name; // reset pointer to the beginning of 'name'
    while (*p != '\0') {
        *(path_buf + path_length) = *p; // copy each character
        path_length++;
        p++;
    }

    // step 5: null-terminate the string & update path_length
    *(path_buf + path_length) = '\0';

    return 0; // success
}


int path_pop() {
    // step 1: check if path_buf is empty
    if (path_length == 0) {
        return -1; // no path to pop
    }

    // step 2: traverse backward to find the first '/'
    char *p = path_buf + path_length - 1; // start at the end of the path

    // first skip any trailing '/' at the end of the path
    while (p >= path_buf && *p == '/') { // make sure p >= path_buf before dereferencing *p
        p--; // move backward past the trailing '/'
    }

    // now, find the last '/' that separates the last component
    while (p >= path_buf && *p != '/') { // make sure p >= path_buf before dereferencing *p
        p--; // move backward until we find '/' or exhaust path
    }

    // step 3: check if we found a '/', or if we exhausted the entire path
    if (p < path_buf) { // the path has been fully traversed
        *path_buf = '\0'; // set path_buf to an empty string
        path_length = 0; // update path_length to reflect an empty path
        return 0; // success
    } else { // found a '/', so truncate the path at that point
        *p = '\0'; // replace the '/' with a null terminator
        path_length = p - path_buf; // update path_length
        return 0; // success
    }
}


int deserialize_directory(int depth) {
   // process each record within the directory
   // in each iteration/record, check if magic bytes are good
   // & check the depth matches for all entries in the same depth
   int record_type;
   while ((record_type = get_record_type(depth)) != -1) {

       if (record_type == 2) { // START_OF_DIRECTORY
           uint64_t record_size = get_8bytes_big2little(); // read the next 8 bytes

           if (record_size == -1) {
               fprintf(stderr, "error. couldn't read record size. \n");
               return -1;
           }

           if (record_size != 16) {
               fprintf(stderr, "error. size of START_OF_DIRECTORY must be 16. \n");
               return -1;
           }

           // START_OF_DIRECTORY size is 16. correct.
           // so we can safely continue to the next record.
           continue;
       }

       if (record_type == 4) { // DIRECTORY_ENTRY
           // step 1: read the size of the record (8 bytes)
           uint64_t record_size = get_8bytes_big2little(); // read the next 8 bytes

           if (record_size == (uint64_t)-1) {
               fprintf(stderr, "error. couldn't read size of DIRECTORY_ENTRY. \n");
               return -1;
           }

           record_size = record_size - 28; //  remove header & metadata bytes. now its just the size of the name
           ////// here, we can check if the size of the name is too big. NAME_MAX
           if (record_size >= NAME_MAX) {
               fprintf(stderr, "error. name size too big.");
               return -1;
           }

           // step 2: read permissions. 4 bytes
           mode_t permissions = (mode_t)get_4bytes_big2little();

           if (permissions == (mode_t)-1) {
               fprintf(stderr, "error. couldn't read permissions from DIRECTORY_ENTRY. \n");
               return -1;
           }

           // step 3: read the file size. 8 bytes
           off_t file_size = (off_t)get_8bytes_big2little();

           if (file_size == (off_t)-1) {
               fprintf(stderr, "error. couldn't read file size. \n");
               return -1;
           }

           // step 4: read the name into path_name
           int read_char;
           for (int i = 0; i < record_size; i++) {
               if ((read_char = getchar()) == EOF) {
                   fprintf(stderr, "error. hit end of file while reading the name to push. \n");
                   return -1;
               }
               *(name_buf + i) = read_char; // write each char of name out on name_buf
           }

           *(name_buf + record_size) = '\0'; // null-terminate the name we're pushing

           // MORE PROCESSING FOR DIRECTORY_ENTRY

           if (path_push(name_buf) == -1) {
               fprintf(stderr, "error. couldn't path_push. \n");
               return -1;
           }

           path_length = strlength(path_buf); // might be redundant but ima still write it just in case

           if (S_ISDIR(permissions)) { // DIRECTORY_ENTRY record is for a new directory
               // try to open the directory
               DIR *dir = opendir(path_buf);
               if (dir) { // directory already exists
                   // check if the clobber flag is present in global_options
                   if (global_options & (1 << 3)) { // -c present. so we skip overwriting
                       // call deserialize_directory for this directory. go one depth deeper
                       return deserialize_directory(depth + 1);
                   }
                   else {
                       fprintf(stderr, "error. directory already exists, but no -c argument. \n");
                       return -1;
                   }
                   if (closedir(dir) == -1) {
                       fprintf(stderr, "error. couldn't set close directory. \n");
                       return -1;
                   }
               }
               else if (errno == ENOENT) { // the directory doesn't exist. so we gotta create it.
                   if (mkdir(path_buf, 0700) == -1) { // create the directory
                       fprintf(stderr, "error. couldn't set make new directory. \n");
                       return -1;
                   }
                   if (chmod(path_buf, permissions & 0777) == -1) { // set the permission for the directory specified by path_buf
                       fprintf(stderr, "error. couldn't set permissions for the file. \n");
                       return -1;
                   }
                   return deserialize_directory(depth + 1); // recursively deserialize this new directory now
               }
               else {
                   return -1;
               }
           }
           else if (S_ISREG(permissions)) { // DIRECTORY_ENTRY record is for a new file
               if (deserialize_file(depth) == -1) {
                   fprintf(stderr, "error. couldn't deserialize_file. \n");
                   return -1;
               }
               if (chmod(path_buf, permissions & 0777) == -1) { // set the permissions for the file specified by path_buf
                   fprintf(stderr, "error. couldn't set permissions for the file. \n");
                   return -1;
               }
               if (path_pop() == -1) { // pop so we can move on to next entry
                   fprintf(stderr, "error. couldn't path_pop. \n");
                   return -1;
               }
           }
           else {
               return -1;
           }
       }
       else if (record_type == 3) { // END_OF_DIRECTORY
           uint64_t record_size = get_8bytes_big2little(); // read the next 8 bytes
           if (record_size == -1) {
               return -1;
           }
           if (record_size != 16) {
               fprintf(stderr, "error. END_OF_DIRECTORY size must be 16. \n");
               return -1;
           }
           if (path_pop() == -1) {
               fprintf(stderr, "error. couldn't path_pop. \n");
               return -1;
           }
           if (depth == 1) {
               return 0;
           }
           return deserialize_directory(depth - 1);
       }
       else { // we didn't catch a START_OF_DIRECTORY, DIRECTORY_ENTRY, or END_OF_DIRECTORY
           fprintf(stderr, "error. no recognized record. \n");
           return -1;
       }
   }
   return -1;
}


int deserialize_file(int depth) {
   int record_type;
   if (access(path_buf, F_OK) == -1) {
       // 1. use get_record_type function to check magic bytes, check depth, and return the type
       // check the returned type to see if it equals 5.

       if ((record_type = get_record_type(depth)) == -1) {
           fprintf(stderr, "error. get_record_type didn't work. \n");
           return -1;
       }

       if (record_type != 5) {
           fprintf(stderr, "error. type should be 5. \n");
           return -1;
       }

       if (create_file() == -1) {
           fprintf(stderr, "error. create_file didn't work. \n");
           return -1;
       }
   }
   else {
       if (global_options & (1 << 3)) { // -c is set. overwrite existing file.

           if ((record_type = get_record_type(depth)) == -1) {
               fprintf(stderr, "error. get_record_type didn't work. \n");
               return -1;
           }

           if (record_type != 5) {
               fprintf(stderr, "error. type should be 5. \n");
               return -1;
           }

           if (create_file() == -1) {
               fprintf(stderr, "error. create_file didn't work. \n");
               return -1;
           }
       }
       else {
           fprintf(stderr, "error. no -c while file exists. \n");
           return -1;
       }
   }
   return 0; // successfully deserialized the file.
}


int serialize_directory(int depth) {
    // write START_OF_DIRECTORY record
    putchar(0x0C); // magic byte 1
    putchar(0x0D); // magic byte 2
    putchar(0xED); // magic byte 3
    putchar(2);    // record type: START_OF_DIRECTORY

    // write the depth (in big-endian) (4 bytes)
    uint32_t be_depth = to_big_endian_32(depth);
    fwrite(&be_depth, sizeof(uint32_t), 1, stdout);

    // write the size (in big-endian) (8 bytes)
    uint64_t size = 16; // size of the record (header only)
    uint64_t be_size = to_big_endian_64(size); // convert to big-endian
    fwrite(&be_size, sizeof(uint64_t), 1, stdout); // write size (in big-endian)

    // open the directory using opendir()
    DIR *dir = opendir(path_buf);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open directory: %s\n", path_buf);
        return -1;
    }

    struct dirent *de;
    struct stat stat_buf;

    // loop over the directory entries
    while ((de = readdir(dir)) != NULL) {
        // skip the "." and ".." entries
        if (strcompare(de->d_name, ".") == 0 || strcompare(de->d_name, "..") == 0) {
            continue;
        }

        // update path_buf to include the current entry (use path_push)
        if (path_push(de->d_name) == -1) {
            fprintf(stderr, "error. Failed to push path: %s\n", de->d_name);
            closedir(dir);
            return -1;
        }

        // get metadata for the current entry using stat()
        if (stat(path_buf, &stat_buf) == -1) {
            fprintf(stderr, "error. Failed to get stat for: %s\n", path_buf);
            closedir(dir);
            return -1;
        }

        // check if it's a directory or file
        if (S_ISDIR(stat_buf.st_mode) || S_ISREG(stat_buf.st_mode)) {
            // write the 16-byte header for DIRECTORY_ENTRY record
            putchar(0x0C); // magic byte 1
            putchar(0x0D); // magic byte 2
            putchar(0xED); // magic byte 3
            putchar(4);    // record type: DIRECTORY_ENTRY

            // write the depth (in big-endian) (4 bytes)
            fwrite(&be_depth, sizeof(uint32_t), 1, stdout);

            // calculate the size of the record (header + metadata + name length) (8 bytes)
            size = 16 + 12 + strlength(de->d_name); // header (16 bytes) + metadata (12 bytes) + name (no null terminator)
            be_size = to_big_endian_64(size);
            fwrite(&be_size, sizeof(uint64_t), 1, stdout);

            // write the 12-byte metadata
            uint32_t mode = to_big_endian_32(stat_buf.st_mode); // mode (permissions + type), 4 bytes
            fwrite(&mode, sizeof(uint32_t), 1, stdout);

            uint64_t file_size = to_big_endian_64(stat_buf.st_size); // size of the file or directory, 8 bytes
            fwrite(&file_size, sizeof(uint64_t), 1, stdout);

            // write the file or directory name (without null terminator)
            fwrite(de->d_name, strlength(de->d_name), 1, stdout);
        }

        // if it's a directory, recursively call serialize_directory
        if (S_ISDIR(stat_buf.st_mode)) {
            if (serialize_directory(depth + 1) == -1) {
                closedir(dir);
                return -1;
            }
        } else if (S_ISREG(stat_buf.st_mode)) {
            // if it's a regular file, call serialize_file
            if (serialize_file(depth, stat_buf.st_size) == -1) {
                closedir(dir);
                return -1;
            }
        }

        // remove the last component from path_buf using path_pop
        if (path_pop() == -1) {
            fprintf(stderr, "Failed to pop path\n");
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);

    // write END_OF_DIRECTORY record
    putchar(0x0C); // magic byte 1
    putchar(0x0D); // magic byte 2
    putchar(0xED); // magic byte 3
    putchar(3);    // record type: END_OF_DIRECTORY

    // write the depth (in big-endian) again
    fwrite(&be_depth, sizeof(uint32_t), 1, stdout);

    // write size (16 for directory) again
    size = 16; // size of the record (header only)
    be_size = to_big_endian_64(size);
    fwrite(&be_size, sizeof(uint64_t), 1, stdout);

    return 0;
}


int serialize_file(int depth, off_t size) {
    // 1. write the 16-byte header for FILE_DATA

    // write magic bytes
    putchar(0x0C); // magic byte 1
    putchar(0x0D); // magic byte 2
    putchar(0xED); // magic byte 3
    putchar(5);    // record type: FILE_DATA

    // write the depth (in big-endian) (4 bytes)
    uint32_t be_depth = to_big_endian_32(depth);
    fwrite(&be_depth, sizeof(uint32_t), 1, stdout);

    // calculate the total size of the record (header + file contents) (8 bytes)
    uint64_t total_size = 16 + size; // 16 bytes header + file size
    uint64_t be_total_size = to_big_endian_64(total_size);
    fwrite(&be_total_size, sizeof(uint64_t), 1, stdout);

    // 2. open the file for reading
    FILE *file = fopen(path_buf, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", path_buf);
        return -1;
    }

    // 3. read the file contents and write them to the output
    off_t total_bytes_written = 0;
    int byte;

    for (off_t i = 0; i < size; i++) {
        byte = fgetc(file);
        if(byte == EOF) {
            fprintf(stderr, "unexpected end of file: %s", path_buf);
            fclose(file);
            return -1;
        }
        if (putchar(byte) == EOF) {
            fprintf(stderr, "failed to write byte to output");
            fclose(file);
            return -1;
        }
        total_bytes_written++;
    }


    // ensure we wrote exactly the expected size
    if (total_bytes_written != size) {
        fprintf(stderr, "Incorrect number of bytes written from file: %s\n", path_buf);
        fclose(file);
        return -1;
    }

    // 4. close the file
    fclose(file);

    return 0;
}


int serialize() {
    // 1. write the START_OF_TRANSMISSION record (16 bytes)

    // write magic bytes
    putchar(0x0C); // magic byte 1
    putchar(0x0D); // magic byte 2
    putchar(0xED); // magic byte 3
    putchar(0);    // record type: START_OF_TRANSMISSION

    // write the depth (always 0 for transmission records) (4 bytes)
    uint32_t be_depth = to_big_endian_32(0);
    fwrite(&be_depth, sizeof(uint32_t), 1, stdout);

    // write the size (16 bytes for the header only, in big-endian) (8 bytes)
    uint64_t be_size = to_big_endian_64(16);
    fwrite(&be_size, sizeof(uint64_t), 1, stdout);

    // 2. serialize the contents of the directory (starting with depth 0)
    if (serialize_directory(1) == -1) {
        fprintf(stderr,"Failed to serialize directory\n");
        return -1;
    }

    // 3. write the END_OF_TRANSMISSION record (16 bytes)

    // write magic bytes
    putchar(0x0C); // magic byte 1
    putchar(0x0D); // magic byte 2
    putchar(0xED); // magic byte 3
    putchar(1);    // record type: END_OF_TRANSMISSION

    // write the depth (always 0 for transmission records) (4 bytes)
    fwrite(&be_depth, sizeof(uint32_t), 1, stdout);

    // write the size (16 bytes for the header only, in big-endian) (8 bytes)
    fwrite(&be_size, sizeof(uint64_t), 1, stdout);

    return 0;
}


int deserialize() {

   DIR *dir = opendir(path_buf);

   if (dir) {
       fprintf(stderr, "error. directory already exists. \n");
       closedir(dir);
   }
   else if (ENOENT == errno) { // directory doesn't exist. create it.
       if (mkdir(path_buf, 0700) == -1) {
           fprintf(stderr, "error. couldn't create directory.\n");
           return -1;
       }
   }
   else {
       fprintf(stderr, "error. couldn't open the specified directory. \n");
       return -1;
   }


   // read the START_OF_DIRECTORY. check magic bytes. check the depth is 0. use returned type value to check if it's 0
   int record_type;
   if ((record_type = get_record_type(0)) == -1) {
       fprintf(stderr, "get_record_type failed for START_OF_DIRECTORY\n");

       return -1;
   }
   if (record_type != 0) {
       fprintf(stderr, "supposed to be START_OF_TRANSMISSION type\n");

       return -1;
   }
   // check the size is 16
   int record_size;
   record_size = get_8bytes_big2little();
   if (record_size != 16) {
       fprintf(stderr, "START_OF_TRANSMISSION size must be 16\n");
       return -1;
   }


   if (deserialize_directory(1) == -1) {
       fprintf(stderr, "error. deserialize_directory resulted in an error.\n");
       return -1;
   }


   //read the END_OF_DIRECTORY. check magic bytes. check the depth is 0. use returned type value to check if it's 1
   if ((record_type = get_record_type(0)) == -1) {
       fprintf(stderr, "get_record_type failed for END_OF_DIRECTORY\n");
       return -1;
   }
   if (record_type != 1) {
       fprintf(stderr, "supposed to be END_OF_TRANSMISSION type\n");
       return -1;
   }
   // check the size is 16
   record_size = get_8bytes_big2little();
   if (record_size != 16) {
       fprintf(stderr, "END_OF_TRANSMISSION size must be 16\n");
       return -1;
   }


   return 0;
}


int validargs(int argc, char **argv) {
    // initialize to 0. this will store the flags we set based on arguments
    global_options = 0;

    // if no arguments, return error
    if (argc == 1) {
        fprintf(stderr, "error. no arguments.\n");
        return -1;
    }

    char *first_arg = *(argv + 1);

    // check if the first argument is "-h"
    if (first_arg && *(first_arg + 0) == '-' && *(first_arg + 1) == 'h') {
        global_options |= 0x1; // set help flag (bit 0)
        return 0; // help flag terminates argument processing
    }

    // check if the first argument is "-s"
    if (first_arg && *(first_arg + 0) == '-' && *(first_arg + 1) == 's') {
        global_options |= 0x2; // set serialize flag (bit 1)

        // check if -s is followed by another flag
        if (argc == 2) {
            // if there are no other arguments, it's valid (-s alone)
            path_init("./"); // default path_buf, when -p DIR not provided
            return 0;
        } else if (argc == 4 && *(argv + 2) && *(*(argv + 2) + 0) == '-' && *(*(argv + 2) + 1) == 'p') {
            // check if the next flag is "-p DIR" rather than another flag following -p
            char *directory = *(argv + 3); // directory argument

            // use path_init to safely assign path_buf & path_length
            if (path_init(directory) == -1) {
                fprintf(stderr, "error. failed path_init\n");
                return -1;
            }
            return 0; // valid. -s followed by -p DIR
        } else {
            // any other flag combination is invalid after -s
            fprintf(stderr, "any other flag combo is invalid after -s\n");
            return -1; // error: invalid flags after -s
        }
    }

    // check if the first argument is "-d"
    if (first_arg && *(first_arg + 0) == '-' && *(first_arg + 1) == 'd') {
        global_options |= 0x4; // set deserialize flag (bit 2)

        // if there are no other arguments, it's valid (-d alone)
        if (argc == 2) {
            path_init("./"); // default path_buf, when -p DIR not provided
            return 0;
        }

        // now, loop through the remaining arguments to check for -c or -p DIR
        int c_flag = 0;
        int p_flag = 0;

        for (int i = 2; i < argc; i++) {
            char *arg = *(argv + i);

            // check for the -c flag
            if (*(arg + 0) == '-' && *(arg + 1) == 'c') {
                if (c_flag) {
                    fprintf(stderr, "error. multiple -c flags.\n");
                    return -1; // multiple -c flags
                }
                global_options |= 0x8; // set clobber flag (bit 3)
                c_flag = 1;
            } else if (*(arg + 0) == '-' && *(arg + 1) == 'p') { // check for the -p DIR flag
                if (p_flag) {
                    fprintf(stderr, "error. multiple -p flags.\n");
                    return -1; // multiple -p flags
                }
                char *directory = *(argv + i + 1);
                if (!directory) { // removed the " || *(directory + 0) == '-' " to include directories that start with -
                    fprintf(stderr, "error. -p not followed by a directory\n");
                    return -1;
                }
                if (path_init(directory) == -1) {
                    fprintf(stderr, "path_init failed\n");
                    return -1; // invalid path
                }
                p_flag = 1;
                i++; // skip the directory argument for the next iteration
            } else {
                fprintf(stderr, "error. -d must be followed by -c or -p DIR.\n");
                return -1;
            }
        }

        // check if there are valid combinations of flags
        if (!p_flag && !c_flag) {
            return 0; // valid: just -d
        } else if (p_flag && !c_flag) {
            return 0; // valid: -d followed only by -p DIR
        } else if (!p_flag && c_flag) {
            path_init("./"); // default path_buf, when -p DIR not provided
            return 0; // valid: -d followed only by -c
        } else if (p_flag && c_flag) {
            return 0; // valid: -d followed by both -p DIR and -c
        } else {
            return -1; // error: invalid combination
        }
    }

    // if no valid flags were found, return error
    return -1;
}
