package ffmpeg.audio;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.os.Binder;

public class play extends Service
{
	audio a;
	H h;
	Bind b;
	OnProgressChangedListener opcl=null;
	public interface OnProgressChangedListener{
		public void OnProgressChanged(int state);
	}
	@Override
	public IBinder onBind(Intent p1)
	{
		// TODO: Implement this method
		return  b;
	}

	@Override
	public void onCreate()
	{
		// TODO: Implement this method
		super.onCreate();
		h=new H();
		a=audio.getInstance(h);
		b=new Bind();
	}
	public void setOnProgressChangedListener(OnProgressChangedListener op){
		opcl=op;
	}
	class H extends Handler
	{

		@Override
		public void handleMessage(Message msg)
		{
			// TODO: Implement this method
			super.handleMessage(msg);
			if(opcl!=null)
				opcl.OnProgressChanged(msg.what);
		}
		
	}
	public class Bind extends Binder{
		public play getplay(){
			return play.this;
		}
		public audio getaudio(){
			return a;
		}
	}
}
