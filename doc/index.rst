.. spellchecker:ignore toctree doxygenfunction doxygenenum undoc
.. spellchecker:ignore doxygenstruct doxygentypedef doxygenvariable

Okdcc Documentation
===================

.. toctree::
   :maxdepth: 2

Logic
-----

Decode
......

.. doxygenstruct:: dcc_Decoder
.. doxygenfunction:: dcc_initializeDecoder
.. doxygenfunction:: dcc_decode

.. doxygenstruct:: dcc_SignalStreamParser
.. doxygenfunction:: dcc_initializeSignalStreamParser
.. doxygenfunction:: dcc_feedSignal

.. doxygenstruct:: dcc_BitStreamParser
.. doxygenfunction:: dcc_initializeBitStreamParser
.. doxygenfunction:: dcc_feedBit

.. doxygenstruct:: dcc_SignalBuffer
.. doxygenfunction:: dcc_initializeSignalBuffer
.. doxygenfunction:: dcc_writeSignalBuffer
.. doxygenfunction:: dcc_readSignalBuffer

.. doxygenfunction:: dcc_parsePacket
.. doxygenfunction:: dcc_parseSpeedAndDirectionPacket
.. doxygenfunction:: dcc_parseAllDecoderResetPacket
.. doxygenfunction:: dcc_parseAllDecoderIdlePacket
.. doxygenfunction:: dcc_parseDecoderResetPacket
.. doxygenfunction:: dcc_parseHardResetPacket
.. doxygenfunction:: dcc_parseDecoderAcknowledgementRequestPacket
.. doxygenfunction:: dcc_parseFactoryTestInstructionPacket
.. doxygenfunction:: dcc_parseConsistControlPacket
.. doxygenfunction:: dcc_parseSpeedStep128ControlPacket

Data
''''

.. doxygentypedef:: dcc_TimeMicroSec
.. doxygentypedef:: dcc_Bit
.. doxygenenum:: dcc_Result
.. doxygenenum:: dcc_StreamParserResult
.. doxygentypedef:: dcc_Address
.. doxygentypedef:: dcc_ConsistAddress
.. doxygenenum:: dcc_Direction
.. doxygentypedef:: dcc_Speed
.. doxygenstruct:: dcc_SpeedAndDirectionPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_DecoderResetPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_HardResetPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_FactoryTestInstructionPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_DecoderFlagsSetPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_AdvancedAddressingSetPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_DecoderAcknowledgementRequestPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_ConsistControlPacket
   :members:
   :undoc-members:
.. doxygenstruct:: dcc_SpeedStep128ControlPacket
   :members:
   :undoc-members:
.. doxygenenum:: dcc_PacketTag
.. doxygenstruct:: dcc_Packet
   :members:
   :undoc-members:
.. doxygenvariable:: dcc_minOneHalfBitSentPeriod
.. doxygenvariable:: dcc_maxOneHalfBitSentPeriod
.. doxygenvariable:: dcc_minOneHalfBitReceivedPeriod
.. doxygenvariable:: dcc_maxOneHalfBitReceivedPeriod
.. doxygenvariable:: dcc_minZeroHalfBitSentPeriod
.. doxygenvariable:: dcc_maxZeroHalfBitSentPeriod
.. doxygenvariable:: dcc_maxStretchedZeroBitPeriod
.. doxygenvariable:: dcc_minZeroHalfBitReceivedPeriod
.. doxygenvariable:: dcc_maxZeroHalfBitReceivedPeriod
.. doxygenvariable:: dcc_maxOneHalfBitSentPeriodDiff
.. doxygenvariable:: dcc_maxOneHalfBitReceivedPeriodDiff
.. doxygenfunction:: dcc_showSignalBuffer
.. doxygenfunction:: dcc_showBytes
.. doxygenfunction:: dcc_showDirection
.. doxygenfunction:: dcc_showSpeedAndDirectionPacket
.. doxygenfunction:: dcc_showPacket
.. doxygenvariable:: dcc_error_log
.. doxygenvariable:: dcc_debug_log
