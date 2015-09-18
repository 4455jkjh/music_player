package ffmpeg.audio;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.os.IBinder;

public class con implements ServiceConnection
{
	play.Bind b;
	list l;
	@Override
	public void onServiceConnected(ComponentName p1, IBinder p2)
	{
		// TODO: Implement this method
		b=(play.Bind)p2;
		l.set(getaudio(),getplay());
	}
	public con(list l){
		this.l=l;
	}
	@Override
	public void onServiceDisconnected(ComponentName p1)
	{
		// TODO: Implement this method
	}
	public audio getaudio(){
		return b.getaudio();
	}
	public play getplay(){
		return b.getplay();
	}
}
