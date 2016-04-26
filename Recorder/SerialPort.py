import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/tty.usbmodem621',
    baudrate=115200,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.SEVENBITS
)

ser.isOpen()

#print 'Enter your commands below.\r\nInsert "exit" to leave the application.'

#input=1
while 1 :
  

  try:
    print ser.readline()
    #time.sleep(1)
  except ser.SerialTimeoutException:
    print('Data could not be read')
    #time.sleep(1)
  #out = ''
  #while ser.inWaiting() > 0:
  #  out += ser.read(1)

  #if out != '':
  #  print out