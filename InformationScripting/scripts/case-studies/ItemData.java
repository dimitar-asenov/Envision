import java.util.ArrayList;
import java.util.List;

public class TopLevel
{
	void test(Item[] items)
	{
		// Do something else


		for (int i = 0; i < items.length; ++i)
		{
			ItemData data = items[i].getData();
			allData.add(data);
			data.detach();
		}
		resetItemCount(items.length - 1);


		// Do some more stuff
	}

	void resetItemCount(int newCount){}

	List<ItemData> allData = new ArrayList<>();
}

class ItemData
{
	void detach(){}
}

class Item
{
	ItemData getData() { return data; }

	ItemData data;
}

// Issues
// #3 Only a single item is supported
// #6 Bug: Can't establish connection using a proxy
// #11 Bug: The app crashes if I try to use the last item

// Commits:
// #4 : Add support for multiple items
// #5 : Fix off-by-one error