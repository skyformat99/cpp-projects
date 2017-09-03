import java.net.URL;
import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Java class which implements the wrapper to the C++ class AsioWave.
 * This class utilizes JNI and native method calls, which wrap to the 
 * JAsioWave.h / JAsioWave.cpp file. The C++ code in that file then
 * calls the appropriate methods on an instantiated AsioWave object.
 * 
 * For more information regarding JNI wrapping as well as a simple
 * example, see the Java Tutorial JNI thread:
 *
 * http://java.sun.com/developer/onlineTraining/Programming/JDCBook/jni.html
 *
 * Note that in this implementation, all native methods begin with the '_' 
 * character, to distinguish from non-native (member) methods.
 *
 * @author Adam Dziorny
 */
public class JAsioWave {

  /**
  * Loads the static library "asioWave.dll", which either resides
  * locally or is copied out of the Jar file into the WINDOWS_SYS 
  * directory.
  */
  static {
    try {
      loadLibraryFromJar("asioWave");
    } catch (Exception ex) {
      throw new RuntimeException("Cannot load native dll: "+ex);
    }
  }

  /** Max number of driver names allowed in String[] **/
  private static final int MAX_DRIVER_NAMES = 32;

  /** Flags for the MIN, MAX or PREFERRED buffer size */
  public static final byte BUFFER_SIZE_MIN = 0;
  public static final byte BUFFER_SIZE_MAX = 1;
  public static final byte BUFFER_SIZE_PREFERRED = 2;

  /** Windows System32 directory, for copying out the .dll file **/
  public static final String WINDOWS_SYS = "c:/windows/system32/";
  

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// CONSTRUCTORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  /**
   * Creates a set of buffers in the AsioWave class, which will
   * be used for writing the tone data to the device. The parameter
   * specifies the buffer size (in samples), which is bounded by
   * the BUFFER_SIZE_MIN and BUFFER_SIZE_MAX values returned from
   * the getBufferSizes method.
   *
   * @param The buffer size, in samples
   * @return True if the buffers were created, false otherwise
   */
  public boolean createBuffers (int bufSize)
  {
    return _createBuffers(bufSize);
  }
  private native boolean _createBuffers (int bufSize);

  /**
   * Displays info about the available input and output channels
   * on the currently-loaded ASIO device. Writes to cout, or the
   * standard output.
   */
  public void displayChannelInfo ()
  {
    _displayChannelInfo();
  }
  private native void _displayChannelInfo ();

  /** 
   * Gathers the requested buffer size from the driver.
   * Note that the driver must be loaded at this point, 
   * otherwise this function will fail (and return -1L).
   *
   * @param sizeFlag A flag for the desired buffer element
   * @return The buffer size, or -1L if an error was returned
   */
  public long getBufferSize (byte sizeFlag)
  {
    if (sizeFlag == BUFFER_SIZE_MIN ||
        sizeFlag == BUFFER_SIZE_MAX ||
        sizeFlag == BUFFER_SIZE_PREFERRED)
      return _getBufferSizes((int)sizeFlag);
    else
      return -1L;
  }
  private native long _getBufferSizes (int index);

  /**
   * Initializes the ASIO system to the driver specified in the
   * string argument. This driver name should have come from the
   * list returned in 'getDriverNames()' method.
   *
   * @param driver A valid ASIO driver to init
   * @return True if initialized, false otherwise
   */
  public boolean initAsioDriver (String driver)
  {
    return _initAsioDriver(driver);
  }
  private native boolean _initAsioDriver (String driver);

  /**
   * Is the underlying AsioWave device ready to present a tone? This
   * is checked by returning (status == BUFFERED) on the underlying
   * object.
   *
   * @return True if it is ready & buffered, false otherwise.
   */
  public boolean isReady ()
  {
    return _isReady();
  }
  private native boolean _isReady ();

  /** 
   * Plays a single tone through the ASIO subsystem, for a given
   * duration, with the linear rampUp ending at rampUp (time) and the
   * linear rampDown starting at rampDown (time).
   *
   * @return True if the sound presentation did not throw an error,
   *         False otherwise.
   */
  public boolean playSound (float amp, float freq, float dur,
    float rampUp, float rampDown, boolean playR, boolean playL)
  {
    return _playSound(amp,freq,dur,rampUp,rampDown,playR,playL);
  }
  private native boolean _playSound (float amp, 
    float freq, float dur, float rampUp, float rampDown,
    boolean playR, boolean playL);

  /**
   * If the initial instantiation of this object was called on a thread
   * that is different from the one which will be calling the 
   * 'play' method, then the registerThread method needs to be called.
   * This initializes the COM model on the current thread.
   */
  public void registerThread ()
  {
    _registerThread();
  }
  private native void _registerThread ();

  /**
   * Deletes and re-instantiates the underlying AsioWave
   * object in C++. This is done to reset the AsioDrivers,
   * create a new _instance variable, and reset the COM initialization.
   */
  public void resetSystem ()
  {
    _resetSystem();
  }
  private native void _resetSystem ();

  /**
   * Sets the sample rate to the new passed rate. First
   * checks to see if this is possible on the device (in 
   * the c++ code), and then changes if possible. Returns
   * the rate being used at the end of this call - either 
   * the new rate if the change was successful, or the old 
   * rate if it wasn't.
   *
   * @param newRate The desired sample rate, in Hz
   * @return The rate in use upon return from this method call
   */
  public float setSampleRate (float newRate)
  {
    return _setSampleRate(newRate);
  }
  private native float _setSampleRate(float newRate);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC STATIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  /**
   * Gets the driver names from the ASIO system, and returns
   * the results in the returned String array.
   *
   * @return The driver names from the ASIO system.
   */
  public static String[] getDriverNames ()
  {
    String[] drivers = new String[MAX_DRIVER_NAMES];
    int numDrivers = _getDriverNames(drivers);

    if (numDrivers == 0)
      return null;

    String[] newDrivers = new String[numDrivers];
    for (int i = 0; i < numDrivers; i++)
      newDrivers[i] = drivers[i];

    return newDrivers;
  }
  private static native int _getDriverNames (String[] names);

  /**
   * Main entry point, written for debugging purposes. Comments
   * provide info on how to use this class.
   */
  public static void main (String[] args) {

    // Instantiate a new class with the default constructor
    JAsioWave w = new JAsioWave();

    // Get a list of the available drivers
    String[] drivers = w.getDriverNames();
    for (int i = 0; i < drivers.length; i++)
      System.out.println(i+": "+drivers[i]);

    // Load the first driver (assuming, as on our debug machine, one exists)
    boolean isLoaded = w.initAsioDriver(drivers[0]);
    System.out.println("Driver was loaded: "+isLoaded);

    // Display info about the channels on this driver
    w.displayChannelInfo();

    // Get the preferred buffer size
    long bufPref = w.getBufferSize(BUFFER_SIZE_PREFERRED);
    System.out.println("Preferred buffer size: " + bufPref);

    // Set the sample rate to 48 kHz
    float newRate = w.setSampleRate(48000.F);
    System.out.println("Sample rate set: "+newRate);

    // Create the buffers for sound presentation
    boolean isCreated = w.createBuffers((int)bufPref);
    System.out.println("Buffers were created: "+isCreated);
    
    // Verify we're ready to present tones
    boolean isReady = w.isReady();
    System.out.println("We are ready? " + isReady);

    // Play a sound through the specified driver
    w.playSound(0.05F,4000.F,1.0F,0.1F,0.9F,true,true);

    try {
      Thread.currentThread().sleep(500);
    } catch (Exception ex) {}

    // Play a second sound through the specified driver
    w.playSound(0.05F,2000.F,1.0F,0.1F,0.9F,true,true);
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  /**
   * This method was an attempt to load the .dll from the Jar file.
   * It seems to be written correctly, but for some reason it is not
   * accurately finding the .dll after copying it to the temp directory.
   *
   * @param libName The name of the library (w/o the .dll) to be loaded
   *
   * @deprecated Not working currently; debug before using
   */
  private static void loadLibraryFromJar (String libName)
    throws IOException
  {
    // Try first the old-fashioned way. If that doesn't work, 
    // then copy out of the jar file.
    try {
      System.loadLibrary(libName);

      return;

    } catch (UnsatisfiedLinkError ex) {}

    // Verify that the tmpdir is on the library path, and add if not
    String libPath = System.getProperty("java.library.path");

    // Find the .dll file inside the .jar file
    String libExtension = ".dll";
    String libFileName = libName + libExtension;

    URL libUrl = ClassLoader.getSystemClassLoader().getResource(libFileName);
    if (libUrl == null)
      throw new RuntimeException("Error: could not find library in jar!");

    // Get a link to the temp file, delete the old, create the new
    File file = new File(WINDOWS_SYS+libFileName);

    if (file.exists())
      file.delete();

    file.createNewFile();

    // Perform the data copy
    InputStream in = libUrl.openStream();
    FileOutputStream out = new FileOutputStream(file);

    int read;
    byte[] buffer = new byte[4096];
    while (in.available() > 0) {
      read = in.read(buffer);
      out.write(buffer, 0, read);
    }
    out.close();
    in.close();

    // And now load the library
    System.loadLibrary(libName);
  }
}

  