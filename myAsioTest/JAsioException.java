/**
 * Exception class for the JAsioWave wrapper.
 *
 * @author Adam Dziorny
 */
public class JAsioException extends RuntimeException 
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// CONSTRUCTORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

  /**
   * These four constructors take the same args as the 
   * superclass constructor, and just pass the arg values
   * to super.
   */
  public JAsioException ()
  {
    super();
  }

  public JAsioException (String msg) 
  {
    super(msg);
  }

  public JAsioException (String msg, Throwable cause)
  {
    super(msg,cause);
  }

  public JAsioException (Throwable cause)
  {
    super(cause);
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PUBLIC METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// PRIVATE METHODS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * 

}