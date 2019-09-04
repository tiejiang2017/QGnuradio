#include "beklBlk/RngLib.h"
#include<stdlib.h>
namespace gr {
  namespace beklBlk {

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

char *beklRngLib::bcopy(const char *src, char *dest, int count)
{
  char *tmp = dest;
  while(count--)
    *tmp++ = *src++;
  return dest;
}
/*******************************************************************************
*
* rngCreate - create an empty ring buffer
*
* This routine creates a ring buffer of size <nbytes>, and initializes
* it.  Memory for the buffer is allocated from the system memory partition.
*
* RETURNS
* The ID of the ring buffer, or NULL if memory cannot be allocated.
*/

bool beklRngLib::rngCreate(
        int nbytes  )
{
    char *buffer;
    rngBuf = (RING_ID) malloc(sizeof (rng));

    if (rngBuf == 0)
        return false;

    /* bump number of bytes requested because ring buffer algorithm
    * always leaves at least one empty byte in buffer */

    buffer = (char *) malloc((unsigned) ++nbytes);

    if (buffer == 0)
    {
        delete  ((char *)rngBuf);
        return false;
    }

    rngBuf->bufSize = nbytes;
    rngBuf->buf	    = buffer;

    rngFlush ();

    return true;
}


/*******************************************************************************
*
* rngFlush - make a ring buffer empty
*
* This routine initializes a specified ring buffer to be empty.
* Any data currently in the buffer will be lost.
*
* RETURNS: N/A
*/

void beklRngLib::rngFlush()
{
    rngBuf->pToBuf   = 0;
    rngBuf->pFromBuf = 0;
}


/*******************************************************************************
*
* rngDelete - delete a ring buffer
*
* This routine deletes a specified ring buffer.
* Any data currently in the buffer will be lost.
*
* RETURNS: N/A
*/

void beklRngLib::rngDelete()
{
    delete  (rngBuf->buf);
    delete ((char *)rngBuf);
}


/*******************************************************************************
*
* rngBufGet - get characters from a ring buffer
*
* This routine copies bytes from the ring buffer <rngId> into <buffer>.
* It copies as many bytes as are available in the ring, up to <maxbytes>.
* The bytes copied will be removed from the ring.
*
* RETURNS:
* The number of bytes actually received from the ring buffer;
* it may be zero if the ring buffer is empty at the time of the call.
*/

int beklRngLib::rngBufGet(
    char *buffer,               /* pointer to buffer to receive data */
    int maxbytes                /* maximum number of bytes to get    */
    )
{
    int bytesgot = 0;
    int pToBuf = rngBuf->pToBuf;
    int bytes2;
    int pRngTmp = 0;

    if (pToBuf >= rngBuf->pFromBuf)
    {
        /* pToBuf has not wrapped around */

        bytesgot = min (maxbytes, pToBuf - rngBuf->pFromBuf);
        bcopy (&rngBuf->buf [rngBuf->pFromBuf], buffer, bytesgot);
        rngBuf->pFromBuf += bytesgot;
    }
    else
    {
        /* pToBuf has wrapped around.  Grab chars up to the end of the
        * buffer, then wrap around if we need to. */

        bytesgot = min (maxbytes, rngBuf->bufSize - rngBuf->pFromBuf);
        bcopy (&rngBuf->buf [rngBuf->pFromBuf], buffer, bytesgot);
        pRngTmp = rngBuf->pFromBuf + bytesgot;

        /* If pFromBuf is equal to bufSize, we've read the entire buffer,
        * and need to wrap now.  If bytesgot < maxbytes, copy some more chars
        * in now. */

        if (pRngTmp == rngBuf->bufSize)
        {
            bytes2 = min (maxbytes - bytesgot, pToBuf);
            bcopy (rngBuf->buf, buffer + bytesgot, bytes2);
            rngBuf->pFromBuf = bytes2;
            bytesgot += bytes2;
        }
        else
            rngBuf->pFromBuf = pRngTmp;
    }
    return (bytesgot);
}


/*******************************************************************************
*
* rngBufPut - put bytes into a ring buffer
*
* This routine puts bytes from <buffer> into ring buffer <ringId>.  The
* specified number of bytes will be put into the ring, up to the number of
* bytes available in the ring.
*
* INTERNAL
* Always leaves at least one byte empty between pToBuf and pFromBuf, to
* eliminate ambiguities which could otherwise occur when the two pointers
* are equal.
*
* RETURNS:
* The number of bytes actually put into the ring buffer;
* it may be less than number requested, even zero,
* if there is insufficient room in the ring buffer at the time of the call.
*/

int beklRngLib::rngBufPut
(               /* ring buffer to put data into  */
 char *buffer,               /* buffer to get data from       */
 int nbytes                   /* number of bytes to try to put */
 )
{
    int bytesput = 0;
    int pFromBuf = rngBuf->pFromBuf;
    int bytes2;
    int pRngTmp = 0;

    if (pFromBuf > rngBuf->pToBuf)
    {
        /* pFromBuf is ahead of pToBuf.  We can fill up to two bytes
        * before it */

        bytesput = min (nbytes, pFromBuf - rngBuf->pToBuf - 1);
        bcopy (buffer, &rngBuf->buf [rngBuf->pToBuf], bytesput);
        rngBuf->pToBuf += bytesput;
    }
    else if (pFromBuf == 0)
    {
        /* pFromBuf is at the beginning of the buffer.  We can fill till
        * the next-to-last element */

        bytesput = min (nbytes, rngBuf->bufSize - rngBuf->pToBuf - 1);
        bcopy (buffer, &rngBuf->buf [rngBuf->pToBuf], bytesput);
        rngBuf->pToBuf += bytesput;
    }
    else
    {
        /* pFromBuf has wrapped around, and its not 0, so we can fill
        * at least to the end of the ring buffer.  Do so, then see if
        * we need to wrap and put more at the beginning of the buffer. */

        bytesput = min (nbytes, rngBuf->bufSize - rngBuf->pToBuf);
        bcopy (buffer, &rngBuf->buf [rngBuf->pToBuf], bytesput);
        pRngTmp = rngBuf->pToBuf + bytesput;

        if (pRngTmp == rngBuf->bufSize)
        {
            /* We need to wrap, and perhaps put some more chars */

            bytes2 = min (nbytes - bytesput, pFromBuf - 1);
            bcopy (buffer + bytesput, rngBuf->buf, bytes2);
            rngBuf->pToBuf = bytes2;
            bytesput += bytes2;
        }
        else
            rngBuf->pToBuf = pRngTmp;
    }
    return (bytesput);
}



/*******************************************************************************
*
* rngIsEmpty - test if a ring buffer is empty
*
* This routine determines if a specified ring buffer is empty.
*
* RETURNS:
* TRUE if empty, FALSE if not.
*/
int beklRngLib::rngIsEmpty
(
 )
{
    return (rngBuf->pToBuf == rngBuf->pFromBuf);
}




/*******************************************************************************
*
* rngIsFull - test if a ring buffer is full (no more room)
*
* This routine determines if a specified ring buffer is completely full.
*
* RETURNS:
* TRUE if full, FALSE if not.
*/

int beklRngLib::rngIsFull
(
 )
{
    int n = rngBuf->pToBuf - rngBuf->pFromBuf + 1;

    return ((n == 0) || (n == rngBuf->bufSize));
}



/*******************************************************************************
*
* rngNBytes - determine the number of bytes in a ring buffer
*
* This routine determines the number of bytes currently in a specified
* ring buffer.
*
* RETURNS: The number of bytes filled in the ring buffer.
*/

int beklRngLib::rngNBytes
( )
{
    int n = rngBuf->pToBuf - rngBuf->pFromBuf;

    if (n < 0)
        n += rngBuf->bufSize;

    return (n);
}



/*******************************************************************************
*
* rngPutAhead - put a byte ahead in a ring buffer without moving ring pointers
*
* This routine writes a byte into the ring, but does not move the ring buffer
* pointers.  Thus the byte will not yet be available to rngBufGet() calls.
* The byte is written <offset> bytes ahead of the next input location in the
* ring.  Thus, an offset of 0 puts the byte in the same position as would
* RNG_ELEM_PUT would put a byte, except that the input pointer is not updated.
*
* Bytes written ahead in the ring buffer with this routine can be made available
* all at once by subsequently moving the ring buffer pointers with the routine
* rngMoveAhead().
*
* Before calling rngPutAhead(), the caller must verify that at least
* <offset> + 1 bytes are available in the ring buffer.
*
* RETURNS: N/A
*/

void beklRngLib::rngPutAhead
(char byte,             /* byte to be put in ring        */
 int offset             /* offset beyond next input byte where to put byte */
 )
{
    int n = rngBuf->pToBuf + offset;

    if (n >= rngBuf->bufSize)
        n -= rngBuf->bufSize;

    *(rngBuf->buf + n) = byte;
}


/*******************************************************************************
*
* rngMoveAhead - advance a ring pointer by <n> bytes
*
* This routine advances the ring buffer input pointer by <n> bytes.  This makes
* <n> bytes available in the ring buffer, after having been written ahead in
* the ring buffer with rngPutAhead().
*
* RETURNS: N/A
*/

void beklRngLib::rngMoveAhead
(
 int n            /* number of bytes ahead to move input pointer */
 )
{
    n += rngBuf->pToBuf;

    if (n >= rngBuf->bufSize)
        n -= rngBuf->bufSize;

    rngBuf->pToBuf = n;
}



int beklRngLib::rngBufGetNoDel (char *buffer, int maxbytes)
{
    int retval;
    int temp=rngBuf->pFromBuf;
    retval=rngBufGet(buffer,  maxbytes);
    rngBuf->pFromBuf=temp;
    return retval;

}

void beklRngLib::rngMoveRead
(
 int n            /* number of bytes ahead to move input pointer */
 )
{
    n += rngBuf->pFromBuf;

    if (n >= rngBuf->bufSize)
        n -= rngBuf->bufSize;

    rngBuf->pFromBuf = n;
}

  }
}
