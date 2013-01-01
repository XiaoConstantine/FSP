
import java.io.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Client {

	
    public byte[] tolh(int n){
		byte[] b = new byte[4];
		b[0] = (byte)(n & 0xff);
		b[1] = (byte)(n >>8 & 0xff);
		b[2] = (byte)(n >>16 & 0xff);
		b[3] = (byte)(n >> 24 & 0xff);
		return b;
	}

    private static String toStr(byte[] valArr,int maxLen) {
        int index = 0;
        while(index < valArr.length && index < maxLen) {
            if(valArr[index] == 0) {
                break;
            }
            index++;
        }
        byte[] temp = new byte[index];
        System.arraycopy(valArr, 0, temp, 0, index);
        return new String(temp);
    }
    
    private static int vtolh(byte[] bArr) {
        int n = 0;
        for(int i=0;i<bArr.length&&i<4;i++){
            int left = i*8;
            n+= (bArr[i] << left);
        }
        return n;
    }
    public String name = "";
    public int id = 0;
    public float salary = 0;
    
    
	public static void main(String[] args) throws IOException {
			
      Socket socket = new Socket("XIAOmatoMacBook-Pro.local", 0x1234);
	  InputStream inputStream = socket.getInputStream();
	  BufferedInputStream bufferedInputStream = new BufferedInputStream(inputStream);
	  byte b[] = new byte[655356];
	  int i = -1;

      

	  while(bufferedInputStream.read(b)!= -1){
		  String s = new String(b);
		  System.out.println("The received message:" + s);
	  }
	}

}
