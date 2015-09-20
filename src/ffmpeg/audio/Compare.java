package ffmpeg.audio;
import java.io.File;
import java.util.Comparator;

public class Compare implements Comparator<File>
{
	public enum method
	{
		name,
		type,
		time,
		size,
	}
	method m;
	boolean dao=false;
	public Compare()
	{
		m = method.name;
	}
	public void setmrthod(method mm)
	{
		m = mm;
	}
	public void setdao(boolean b)
	{
		dao = b;
	}
	@Override
	public int sortByname(File p1, File p2)
	{
		// TODO: Implement this method
		if (p1.isDirectory() & p2.isFile())return -1;
		if (p1.isFile() & p2.isDirectory())return 1;
		return p1.getName().toLowerCase().compareTo(p2.getName().toLowerCase());
	}
	public int sortBytype(File p1, File p2)
	{
		return gettype(p1).compareTo(gettype(p2));
	}
	public String gettype(File f)
	{
		return f.getName().substring(f.getName().lastIndexOf(".") + 1);
	}
	public int sortBysize(File p1, File p2)
	{
		long l=p1.length() - p2.length();
		if (l > 0)
			return 1;
		else if (l < 0)
			return -1;
		else
			return 0;
	}
	public int sortBytime(File p1, File p2)
	{
		long l=p2.lastModified() - p1.lastModified();
		if (l > 0)
			return 1;
		else if (l < 0)
			return -1;
		else
			return 0;
	}
	
	@Override
	public int compare(File p1, File p2)
	{
		// TODO: Implement this method
		int ret=0;
		switch (m)
		{
			case type:
				ret = sortBytype(p1, p2);
				break;
			case size:
				ret = sortBysize(p1, p2);
				break;
			case time:
				ret = sortBytime(p1, p2);
				break;
			case name:
			default:
				ret = sortByname(p1, p2);
		}
		if (dao)
			ret = -ret;
		return ret;
	}
}
