import java.math.BigInteger;

public class BigJavaDemo
{
	public static void main(String args[])
	{
		int val = 71, power = 71117;
		BigInteger result = (new BigInteger(Integer.toString(val))).pow(power);
		System.out.println(String.format("%d^%d is %s", val, power, result));
	}
}