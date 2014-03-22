#include "crc.h"

long get_file_size(const char *filename) {
    FILE *input_file;
    long file_size;

    input_file = fopen(filename, "r");
    if(input_file == NULL) { 
        LERROR(0, 0, "Could not open %s for reading", filename);
        return 0;
    }
    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    rewind(input_file);
    fclose(input_file);
    return file_size;
}

uint32_t compute_crc32(const char *filename) { 
    int input_fd;
    Bytef *file_buffer;
    long file_size;
    unsigned long chunk_size=1024;
    uInt bytes_read;
    unsigned long crc = 0;
  
    file_size = get_file_size(filename);
    if(file_size == 0) { 
        LERROR(0, 0, "%s: file size is 0. Cannot calculate CRC", filename);
        return 0;
    }
    file_buffer = malloc(chunk_size);
    if(file_buffer == NULL) { 
        LERROR(0, 0, "buffer allocation error");
        return 0;
    }
    crc = crc32(0L, Z_NULL, 0); 
    input_fd = open(filename, O_RDONLY);
    if(input_fd == -1) { 
        LERROR(0, 0, "could not open %s for reading", filename);
        return 0;
    }
    while(1) {
        bytes_read = read(input_fd, file_buffer, chunk_size);
        if(bytes_read == 0)
            break;
        if(bytes_read == -1) {
            LERROR(0, errno, "read() returned -1");
            break;
        }
        crc = crc32(crc, file_buffer, bytes_read);
    }
    free(file_buffer);
    close(input_fd);
    return (uint32_t) crc;
}


