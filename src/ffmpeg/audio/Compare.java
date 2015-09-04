package ffmpeg.audio;
import java.io.File;
import java.util.Comparator;

public class Compare implements Comparator<File>
{

	@Override
	public int compare(File p1, File p2)
	{
		// TODO: Implement this method
		if(p1.isDirectory()&p2.isFile())return -1;
		if(p1.isFile()&p2.isDirectory())return 1;
		return p1.getName().toLowerCase().compareTo(p2.getName().toLowerCase());
	}
	
}
