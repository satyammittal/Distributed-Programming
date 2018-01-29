const MAXLEN = 1024;
 
/*
 * Type for storing path
 */
typedef string filename<MAXLEN>;
typedef char String<MAXLEN>;
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
 * File data sent by the server from client to store
 * it on the server along with the filename and the
 * number of bytes in the data
 */
struct chunksend {
    filename name;
    filechunk data;
    String pattern;
    int flag;
    int bytes;
};


/*
 * Type that represents the structure for file's chunks
 * to be sent to the server 
 */
typedef struct chunksend chunksend;



program MATHPROG {
    version MATHVERS {
        String sendfile(chunksend *) = 1;
    } = 1;
} = 0x20000008;
