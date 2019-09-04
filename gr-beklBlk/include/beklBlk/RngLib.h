#ifndef RNGLIB_H
#define RNGLIB_H



namespace gr {
  namespace beklBlk {

  typedef struct
  {
      int pToBuf;		/* offset from start of buffer where to write next */
      int pFromBuf;	/* offset from start of buffer where to read next */
      int bufSize;	/* size of ring in bytes */
      char *buf;
  }rng;
  typedef  rng* RING_ID;
    /*!
     * \brief <+description of block+>
     * \ingroup beklBlk
     *
     */
    class   beklRngLib
    {
     public:
        beklRngLib(){}
        char *bcopy(const char *src, char *dest, int count);
        void    rngFlush ();

        void    rngDelete();

        int     rngBufGet(
                char *buffer,               /* pointer to buffer to receive data */
                int maxbytes                /* maximum number of bytes to get    */
                );

         bool rngCreate(int nbytes  );

        int rngBufPut (char *buffer,               /* buffer to get data from       */
         int nbytes                   /* number of bytes to try to put */
         );

        int rngIsEmpty( );

        int rngIsFull();

        int rngNBytes();


        void rngPutAhead( char byte,             /* byte to be put in ring        */
            int offset             /* offset beyond next input byte where to put byte */
        );

        void rngMoveAhead
        (
         int n            /* number of bytes ahead to move input pointer */
        );

        int rngBufGetNoDel ( char *buffer, int maxbytes);

        void rngMoveRead
        (

         int n            /* number of bytes ahead to move input pointer */
        );
     private:
       RING_ID rngBuf;

    };

  } // namespace beklBlk
} // namespace gr


#endif // RINGBUFFER_H
