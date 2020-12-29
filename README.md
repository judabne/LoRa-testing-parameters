# LoRa-testing-parameters
I created a sender and a receiver to test the performance of different LoRa parameters on TTGO LoRa32-OLED V1
The code is modified from the examples that come initially with the LoRa library.

This does not abide by the allowed time on air but the testing took place indoors in a building with reinforced concrete,
it didn't therefore interfere with nearby devices.

The sender sends a chain of 8 characters, while the receiver logs all the packets it received.
I also created an algorithm that allows changing parameters in an automatic way using the PRM variable
(here the change occurs every 500 packets, to obtain sound results you need to increase them).

The project should work perfectly with a sender and receiver. I created a relay however.
The relay can be placed between the sender and receiver. In case the receiver fails to receive the 
parameter change signal from the sender, the relay will receive a signal and issue it to the receiver.

The parameter change algorithm is designed to happen once at the receiver for every parameter combination
even with multiple signal-change messages issued. Therefore, using the relay, or increasing the signal-change messages
sent should not cause conflicts.

Judy Abi Nehme
