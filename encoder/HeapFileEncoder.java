/*  MIT License
 *
 *  Copyright (c) 2022 MIT-DB-Class
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

import java.io.*;
import java.util.Arrays;
import java.util.List;

/**
 * HeapFileEncoder reads a comma delimited text file or accepts
 * an array of tuples and converts it to
 * pages of binary data in the appropriate format for simpledb heap pages
 * Pages are padded out to a specified length, and written consecutive in a
 * data file.
 */

public class HeapFileEncoder {

    /**
     * Class representing a type in SimpleDB.
     * Types are static objects defined by this class; hence, the Type
     * constructor is private.
     */
    public enum Type implements Serializable {
        INT_TYPE() {
            @Override
            public int getLen() {
                return 4;
            }

        }, STRING_TYPE() {
            @Override
            public int getLen() {
                return STRING_LEN + 4;
            }
        };
        public static final int STRING_LEN = 128;
        public abstract int getLen();
    }

    public static void main(String[] args) {
        try {
            if (args.length < 2 || args.length > 4) {
                System.err.println("Unexpected number of arguments to convert ");
                return;
            }
            File sourceTxtFile = new File(args[0]);
            File targetDatFile = new File(args[0].replaceAll(".txt", ".dat"));
            int numOfAttributes = Integer.parseInt(args[1]);
            Type[] ts = new Type[numOfAttributes];
            char fieldSeparator = ',';

            if (args.length == 2)
                for (int i = 0; i < numOfAttributes; i++)
                    ts[i] = Type.INT_TYPE;
            else {
                String typeString = args[2];
                String[] typeStringAr = typeString.split(",");
                if (typeStringAr.length != numOfAttributes) {
                    System.err.println("The number of types does not agree with the number of columns");
                    return;
                }
                int index = 0;
                for (String s : typeStringAr) {
                    if (s.equalsIgnoreCase("int"))
                        ts[index++] = Type.INT_TYPE;
                    else if (s.equalsIgnoreCase("string"))
                        ts[index++] = Type.STRING_TYPE;
                    else {
                        System.err.println("Unknown type " + s);
                        return;
                    }
                }
                if (args.length == 5)
                    fieldSeparator = args[3].charAt(0);
            }

            HeapFileEncoder.convert(sourceTxtFile, targetDatFile,
                    4096, numOfAttributes, ts, fieldSeparator);

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }


    /**
     * Convert the specified tuple list (with only integer fields) into a binary
     * page file. <br>
     * <p>
     * The format of the output file will be as specified in HeapPage and
     * HeapFile.
     *
     * @param tuples     the tuples - a list of tuples, each represented by a list of integers that are
     *                   the field values for that tuple.
     * @param outFile    The output file to write data to
     * @param npagebytes The number of bytes per page in the output file
     * @param numFields  the number of fields in each input tuple
     * @throws IOException if the temporary/output file can't be opened
     * @see HeapPage
     * @see HeapFile
     */
    public static void convert(List<List<Integer>> tuples, File outFile, int npagebytes, int numFields) throws IOException {
        File tempInput = File.createTempFile("tempTable", ".txt");
        tempInput.deleteOnExit();
        BufferedWriter bw = new BufferedWriter(new FileWriter(tempInput));
        for (List<Integer> tuple : tuples) {
            int writtenFields = 0;
            for (Integer field : tuple) {
                writtenFields++;
                if (writtenFields > numFields) {
                    throw new RuntimeException("Tuple has more than " + numFields + " fields: (" +
                            listToString(tuple) + ")");
                }
                bw.write(String.valueOf(field));
                if (writtenFields < numFields) {
                    bw.write(',');
                }
            }
            bw.write('\n');
        }
        bw.close();
        convert(tempInput, outFile, npagebytes, numFields);
    }

    public static void convert(File inFile, File outFile, int npagebytes,
                               int numFields) throws IOException {
        Type[] ts = new Type[numFields];
        Arrays.fill(ts, Type.INT_TYPE);
        convert(inFile, outFile, npagebytes, numFields, ts);
    }

    public static void convert(File inFile, File outFile, int npagebytes,
                               int numFields, Type[] typeAr)
            throws IOException {
        convert(inFile, outFile, npagebytes, numFields, typeAr, ',');
    }

    /**
     * Convert the specified input text file into a binary
     * page file. <br>
     * Assume format of the input file is (note that only integer fields are
     * supported):<br>
     * int,...,int\n<br>
     * int,...,int\n<br>
     * ...<br>
     * where each row represents a tuple.<br>
     * <p>
     * The format of the output file will be as specified in HeapPage and
     * HeapFile.
     *
     * @param inFile     The input file to read data from
     * @param outFile    The output file to write data to
     * @param npagebytes The number of bytes per page in the output file
     * @param numFields  the number of fields in each input line/output tuple
     * @throws IOException if the input/output file can't be opened or a
     *                     malformed input line is encountered
     * @see HeapPage
     * @see HeapFile
     */
    public static void convert(File inFile, File outFile, int npagebytes,
                               int numFields, Type[] typeAr, char fieldSeparator)
            throws IOException {

        int nrecbytes = 0;
        for (int i = 0; i < numFields; i++) {
            nrecbytes += typeAr[i].getLen();
        }
        int nrecords = (npagebytes * 8) / (nrecbytes * 8 + 1);  //floor comes for free

        //  per record, we need one bit; there are nrecords per page, so we need
        // nrecords bits, i.e., ((nrecords/32)+1) integers.
        int nheaderbytes = (nrecords / 8);
        if (nheaderbytes * 8 < nrecords)
            nheaderbytes++;  //ceiling
        int nheaderbits = nheaderbytes * 8;

        BufferedReader br = new BufferedReader(new FileReader(inFile));
        FileOutputStream os = new FileOutputStream(outFile);

        // our numbers probably won't be much larger than 1024 digits
        char[] buf = new char[1024];

        int curpos = 0;
        int recordcount = 0;
        int npages = 0;
        int fieldNo = 0;

        ByteArrayOutputStream headerBAOS = new ByteArrayOutputStream(nheaderbytes);
        DataOutputStream headerStream = new DataOutputStream(headerBAOS);
        ByteArrayOutputStream pageBAOS = new ByteArrayOutputStream(npagebytes);
        DataOutputStream pageStream = new DataOutputStream(pageBAOS);

        boolean done = false;
        boolean first = true;
        while (!done) {
            int c = br.read();

            // Ignore Windows/Notepad special line endings
            if (c == '\r')
                continue;

            if (c == '\n') {
                if (first)
                    continue;
                recordcount++;
                first = true;
            } else
                first = false;
            if (c == fieldSeparator || c == '\n' || c == '\r') {
                String s = new String(buf, 0, curpos);
                if (typeAr[fieldNo] == Type.INT_TYPE) {
                    try {
                        pageStream.writeInt(Integer.parseInt(s.trim()));
                    } catch (NumberFormatException e) {
                        System.out.println("BAD LINE : " + s);
                    }
                } else if (typeAr[fieldNo] == Type.STRING_TYPE) {
                    s = s.trim();
                    int overflow = Type.STRING_LEN - s.length();
                    if (overflow < 0) {
                        s = s.substring(0, Type.STRING_LEN);
                    }
                    pageStream.writeInt(s.length());
                    pageStream.writeBytes(s);
                    while (overflow-- > 0)
                        pageStream.write((byte) 0);
                }
                curpos = 0;
                if (c == '\n')
                    fieldNo = 0;
                else
                    fieldNo++;

            } else if (c == -1) {
                done = true;

            } else {
                buf[curpos++] = (char) c;
                continue;
            }

            // if we wrote a full page of records, or if we're done altogether,
            // write out the header of the page.
            //
            // in the header, write a 1 for bits that correspond to records we've
            // written and 0 for empty slots.
            //
            // when we're done, also flush the page to disk, but only if it has
            // records on it.  however, if this file is empty, do flush an empty
            // page to disk.
            if (recordcount >= nrecords
                    || done && recordcount > 0
                    || done && npages == 0) {
                int i = 0;
                byte headerbyte = 0;

                for (i = 0; i < nheaderbits; i++) {
                    if (i < recordcount)
                        headerbyte |= (1 << (i % 8));

                    if (((i + 1) % 8) == 0) {
                        headerStream.writeByte(headerbyte);
                        headerbyte = 0;
                    }
                }

                if (i % 8 > 0)
                    headerStream.writeByte(headerbyte);

                // pad the rest of the page with zeroes

                for (i = 0; i < (npagebytes - (recordcount * nrecbytes + nheaderbytes)); i++)
                    pageStream.writeByte(0);

                // write header and body to file
                headerStream.flush();
                headerBAOS.writeTo(os);
                pageStream.flush();
                pageBAOS.writeTo(os);

                // reset header and body for next page
                headerBAOS = new ByteArrayOutputStream(nheaderbytes);
                headerStream = new DataOutputStream(headerBAOS);
                pageBAOS = new ByteArrayOutputStream(npagebytes);
                pageStream = new DataOutputStream(pageBAOS);

                recordcount = 0;
                npages++;
            }
        }
        br.close();
        os.close();
    }

    public static String listToString(List<Integer> list) {
        StringBuilder out = new StringBuilder();
        for (Integer i : list) {
            if (out.length() > 0) out.append("\t");
            out.append(i);
        }
        return out.toString();
    }
}
