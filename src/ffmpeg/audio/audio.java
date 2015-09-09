package ffmpeg.audio;
import android.os.Handler;

public class audio
{
	Handler h;
	static{
		System.loadLibrary("audio");
	}
	public native void init();
	public native int setData(String s);
	public native void play();
	public native void release();
	public native boolean isPlaying();
	public native void pause();
	public native long getcur();
	public native long gettotal();
	public native String getinfo(String s);
	public void callback(int i){
		if(h!=null)
			h.sendEmptyMessage(i);
	}
	public audio(Handler h){
		init();
		this.h=h;
	}
}
