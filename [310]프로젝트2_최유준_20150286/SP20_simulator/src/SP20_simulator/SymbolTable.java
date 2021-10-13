package SP20_simulator;
import java.util.ArrayList;

/**
 * symbol�� ���õ� �����Ϳ� ������ �����Ѵ�.
 * section ���� �ϳ��� �ν��Ͻ��� �Ҵ��Ѵ�.
 */
public class SymbolTable {
	ArrayList<String> symbolList;
	ArrayList<Integer> addressList;
	// ��Ÿ literal, external ���� �� ó������� �����Ѵ�.
	// ���� ����� ���� ����Ʈ
	ArrayList<Integer> modifSizeList;
	// ���� ���, �� ��ȣ�� ���� ����Ʈ
	ArrayList<Character> modifModeList;
	// ������ ���� ���α׷� ��ȣ�� ���� ����Ʈ
	ArrayList<Integer> sectionList;

	public SymbolTable()
	{
		symbolList = new ArrayList<>();
		addressList = new ArrayList<>();
		modifSizeList = new ArrayList<>();
		modifModeList = new ArrayList<>();
		sectionList = new ArrayList<>();
	}
	/**
	 * ���ο� Symbol�� table�� �߰��Ѵ�.
	 * @param symbol : ���� �߰��Ǵ� symbol�� label
	 * @param address : �ش� symbol�� ������ �ּҰ�
	 * <br><br>
	 * ���� : ���� �ߺ��� symbol�� putSymbol�� ���ؼ� �Էµȴٸ� �̴� ���α׷� �ڵ忡 ������ ������ ��Ÿ����. 
	 * ��Ī�Ǵ� �ּҰ��� ������ modifySymbol()�� ���ؼ� �̷������ �Ѵ�.
	 */
	public void putSymbol(String symbol, int address) {
		String inputSymbol = symbol;
		if (!symbolList.contains(inputSymbol))
		{
			// �ɺ��� ���ڷ� ���� �ּҰ��� ������
			symbolList.add(inputSymbol);
			addressList.add(address);
		}

	}
	
	/**
	 * ������ �����ϴ� symbol ���� ���ؼ� ����Ű�� �ּҰ��� �����Ѵ�.
	 * @param symbol : ������ ���ϴ� symbol�� label
	 * @param newaddress : ���� �ٲٰ��� �ϴ� �ּҰ�
	 */
	public void modifySymbol(String symbol, int newaddress) {
		String inputSymbol = symbol;

		// List �� �̹� ����Ǿ��ִ� ��쿡�� ������ ����
		if (symbolList.contains(inputSymbol))
		{
			// ����Ǿ��ִ� �ɺ��� ��ġ�� ã�� ���ڷ� ���� ���ο� �ּҰ��� �־���
			for (int index = 0; index < symbolList.size(); index++)
				if (inputSymbol.equals(symbolList.get(index)))
				{
					symbolList.set(index, inputSymbol);
					addressList.set(index, newaddress);
					break;
				}
		}
	}
	
	/**
	 * ���ڷ� ���޵� symbol�� � �ּҸ� ��Ī�ϴ��� �˷��ش�. 
	 * @param symbol : �˻��� ���ϴ� symbol�� label
	 * @return symbol�� ������ �ִ� �ּҰ�. �ش� symbol�� ���� ��� -1 ����
	 */
	public int search(String symbol) {
		// ����� �ּҰ� ����
		int address = 0;

		// ���ڷ� ���� �ɺ��� List �� �ִ� ���
		// �ش� �ɺ��� �ּҰ��� ã�� address�� ����
		if (symbolList.contains(symbol))
		{
			for (int index = 0; index < symbolList.size(); index++)
				if (symbol.equals(symbolList.get(index)))
				{
					address = addressList.get(index);
					break;
				}
		}
		// ���� ��� -1�� address�� ����
		else
			address = -1;

		// address ����
		return address;
	}
	
	public void putExSymbol(String symbol, int address, int modifSize, char modifMode, int section)
	{
		symbolList.add(symbol);
		addressList.add(address);
		modifSizeList.add(modifSize);
		modifModeList.add(modifMode);
		sectionList.add(section);
	}

	public int size()
	{
		return symbolList.size();
	}
	
	public String getSymbol(int index)//�ɺ� ��ȯ
	{
		return symbolList.get(index);
	}

	public int getaddress(int index)//�ּ� ��ȯ
	{
		return addressList.get(index);
	}

	public int getModifSize(int index)//������ ������ ��ȯ
	{
		return modifSizeList.get(index);
	}

	public char getModifMode(int index)//���� �� ��ȯ
	{
		return modifModeList.get(index);
	}

	public int getSection(int index)//���� ��ȯ
	{
		return sectionList.get(index);
	}
}
