#ifndef INCLUDED_MSG_ACCEPTER_H
#define INCLUDED_MSG_ACCEPTER_H

#include <gnuradio/api.h>
#include <pmt/pmt.h>
#include <boost/shared_ptr.hpp>

namespace gr {
namespace messages {

/*!
 * \brief Virtual base class that accepts messages
 */
class GR_RUNTIME_API msg_accepter
{
public:
    msg_accepter(){};
    virtual ~msg_accepter();

    /*!
     * \brief send \p msg to \p msg_accepter on port \p which_port
     *
     * Sending a message is an asynchronous operation.  The \p post
     * call will not wait for the message either to arrive at the
     * destination or to be received.
     */
    virtual void post(pmt::pmt_t which_port, pmt::pmt_t msg) = 0;
};

typedef boost::shared_ptr<msg_accepter> msg_accepter_sptr;

} /* namespace messages */
} /* namespace gr */

#endif /* INCLUDED_MSG_ACCEPTER_H */
