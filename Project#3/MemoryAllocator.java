/*
+---------------------------------------------+
|   MemoryAllocator                           |
+---------------------------------------------+
| -memory: List<MemoryBlock>                  |
+---------------------------------------------+
| +MemoryAllocator()                          |
| +allocateMemory(String, int): void          |
| +releaseMemory(String): void                |
| +printMemory(): void                        |
| -calculateTargetSize(int): int              |
| -splitBlock(MemoryBlock, int, String): void |
| -mergeAdjacentFreeBlocks(): void            |
+---------------------------------------------+
*/

import java.util.ArrayList;
import java.util.List;

public class MemoryAllocator {
	/* Class constructor variables */
    private List<MemoryBlock> memory;
    /* Class constructor variables */

    /* Constructor for the MemoryAllocator class */
    public MemoryAllocator() {
        memory = new ArrayList<>();
        memory.add(new MemoryBlock(1024, ""));
    }

    /* Method to allocate memory with the given name and size */
    public void allocateMemory(String name, int size) {
        int targetSize = calculateTargetSize(size);
        for (MemoryBlock block : memory) {
            if (block.getSize() >= targetSize && block.getName().isEmpty()) {
                splitBlock(block, targetSize, name);
                break;
            }
        }
    }

    /* Method to release memory with the given name */
    public void releaseMemory(String name) {
        for (MemoryBlock block : memory) {
            if (block.getName().equals(name)) {
                block.setName("");
                mergeAdjacentFreeBlocks();
                break;
            }
        }
    }

    /* Method to calculate the target size for a memory request */
    private int calculateTargetSize(int size) {
        int targetSize = 64;
        while (targetSize < size) {
            targetSize *= 2;
        }
        return targetSize;
    }

    /* Method to split a memory block to satisfy a memory request */
    private void splitBlock(MemoryBlock block, int targetSize, String name) {
        while (block.getSize() > targetSize) {
            int newSize = block.getSize() / 2;
            MemoryBlock newBlock = new MemoryBlock(newSize, "");
            memory.add(memory.indexOf(block) + 1, newBlock);
            block.setSize(newSize);
        }
        block.setName(name);
    }

    /* Method to merge adjacent free memory blocks */
    private void mergeAdjacentFreeBlocks() {
        boolean didMerge = false;
        for (int i = 0; i < memory.size() - 1; i++) {
            MemoryBlock current = memory.get(i);
            MemoryBlock next = memory.get(i + 1);
            if (current.getName().isEmpty() && next.getName().isEmpty() && current.getSize() == next.getSize()) {
                current.setSize(current.getSize() * 2);
                memory.remove(i + 1);
                didMerge = true;
            }
        }
        
        if (didMerge) { mergeAdjacentFreeBlocks(); }
    }

    /* Method to print the current memory state */
    public void printMemory() {
        StringBuilder sb = new StringBuilder();
        sb.append("--------------------------------------------------\n");
        for (MemoryBlock block : memory) {
            sb.append("| ");
            if (!block.getName().isEmpty()) {
                sb.append(block.getName());
                sb.append(" ");
            }
            sb.append(String.format("%5d", block.getSize()));
            sb.append("K ");
        }
        sb.append("|\n--------------------------------------------------");
        System.out.println(sb.toString());
    }
} // MemoryAllocator

