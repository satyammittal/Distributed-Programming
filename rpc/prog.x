const MAXLEN = 1024;
 
/*
 * Type for storing path
 */
typedef string filename<MAXLEN>;

/*
 * Structure for sending request. Expects the path of the file
 * and the byte number at which to start reading the file from
 */
struct request {
    filename name;
    int start;
};

/*
 * Type that represents the structute for request
 */
typedef struct request request;

/*
 * Type for storing a chunk of the file that is being
 * sent from the server to the client in the current
 * remote procedure call
 */
typedef opaque filechunk[MAXLEN];

/*
 * Response sent by the server to the client as a response
 * to remote procedure call, containing the filechunk for
 * the current call and number of bytes actually read
 */
struct chunkreceive {
    filechunk data;
    int bytes;
};

/*
 * Type that represents the structure for file's chunks
 * to be received from the server
 */
typedef struct chunkreceive chunkreceive;

/*
 * File data sent by the server from client to store
 * it on the server along with the filename and the
 * number of bytes in the data
 */
struct chunksend {
    filename name;
    filechunk data;
    int bytes;
};

/*
 * Type that represents the structure for file's chunks
 * to be sent to the server 
 */
typedef struct chunksend chunksend;

/*
 * union for returning from remote procedure call, returns
 * the proper chunkdata response if everything worked fine
 * or will return the error number if an error occured
 */
union readfile_res switch (int errno) {
    case 0:
        chunkreceive chunk;
    default:
        void;
};


struct intpair {
    int a;
    int b;
};

typedef char String<MAXLEN>;

program MATHPROG {
    version MATHVERS {
        int ADD(intpair) = 1;
        int SUB(intpair) = 2;
        String SENDSTRING(void) = 3;
        String SENDSTRINGS(String) = 4;
        readfile_res retrievefile(request *) = 5;
        String sendfile(chunksend *) = 6;
    } = 1;
} = 0x20000008;
