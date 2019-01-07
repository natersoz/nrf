''' Calculate advertising packet loss based on the Poission distribution '''
import math

# Link Layer packet lengths.
PREAMBLE_LENGTH         =  1
ACCESS_ADDRESS_LENGTH   =  4
HEADER_LENGTH           =  1
LENGTH_LENGTH           =  1
PAYLOAD_LENGTH          = 37    # 6 + Advertising data bytes
CRC_LENGTH              =  3

# Link Layer Packet Length
LINK_LAYER_PACKET_LENGTH = (PREAMBLE_LENGTH +
                            ACCESS_ADDRESS_LENGTH +
                            HEADER_LENGTH +
                            LENGTH_LENGTH +
                            PAYLOAD_LENGTH +
                            CRC_LENGTH)

# For a 1 Mbps data rate the symbol period:
BLE_SYMBOL_PERIOD = 1.0E-6

# The Data Packet duration, this is the period of one symbol multiplied
# by the number of bits (bytes * 8) in a packet.
LINK_LAYER_PACKET_PERIOD = BLE_SYMBOL_PERIOD * LINK_LAYER_PACKET_LENGTH * 8

BEACON_COUNT                = 100
BEACON_ADVERTISING_INTERVAL = 500E-3
SOFTWARE_LATENCY            = 1500.0E-6

def poisson(k, lamb):
    '''
    Calculate the Poisson distribution given (k, lambda).
    '''
    return math.exp(-lamb) * math.pow(lamb, k) / math.factorial(k)

def main():
    ''' Calculate advertising packet loss given the encoded above parameters '''

    print("Beacon Count             = {}".format(BEACON_COUNT))
    print("Advertising Interval     = {:.0f} msec".format(BEACON_ADVERTISING_INTERVAL * 1000))
    print("Link Layer Packet Length = {} bytes, {} bits".format(LINK_LAYER_PACKET_LENGTH, LINK_LAYER_PACKET_LENGTH*8))
    print("Link Layer Packet Period = {:.0f} usec".format(LINK_LAYER_PACKET_PERIOD * 1.0E6))

    p_packet_none = 0.0     # The probability that no packet is received or lost.
    p_packet_recv = 0.0     # The probability that a packet was received.
    p_packet_loss = 0.0     # The probability that a packet was lost

    lamb = (LINK_LAYER_PACKET_PERIOD + SOFTWARE_LATENCY) / (BEACON_ADVERTISING_INTERVAL / BEACON_COUNT)
    for k in range(0, 10):
        p_k = poisson(k, lamb)
        if k == 0:
            p_packet_none = p_k
        elif k == 1:
            p_packet_recv = p_k
        elif k > 1:
            p_packet_loss += p_k
        print("P({:3d}, {:.3f}) = {:.3e}".format(k, lamb, p_k))

    print("")
    print("P packet none = {:.4e}".format(p_packet_none))
    print("P packet recv = {:.4e}".format(p_packet_recv))
    print("P packet lost = {:.4e}".format(p_packet_loss))

    ratio_packet_recv = p_packet_recv / (p_packet_recv + p_packet_loss)
    ratio_packet_loss = 1.0 - ratio_packet_recv
    print("percent packet loss = {:.1f}".format(ratio_packet_loss * 100.0))

if __name__ == '__main__':
    main()
