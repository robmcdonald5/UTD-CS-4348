/*
+---------------------------+
|   MemoryBlock             |
+---------------------------+
| -size: int                |
| -name: String             |
+---------------------------+
| +MemoryBlock(int, String) |
| +getSize(): int           |
| +setSize(int): void       |
| +getName(): String        |
| +setName(String): void    |
+---------------------------+
*/

public class MemoryBlock {
	/* Class constructor variables */
    private int size;
    private String name;
    /* Class constructor variables */

    /* Constructor for the MemoryBlock class */
    public MemoryBlock(int size, String name) { this.size = size; this.name = name; }

    /* Getter for the size of the memory block */
    public int getSize() { return size; }

    /* Setter for the size of the memory block */
    public void setSize(int size) { this.size = size; }

    /* Getter for the name of the memory block */
    public String getName() { return name; }

    /* Setter for the name of the memory block */
    public void setName(String name) { this.name = name; }
} // MemoryBlock
