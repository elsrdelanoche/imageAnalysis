package harris;

import java.awt.*;
import java.awt.color.ColorSpace;
import java.awt.image.BufferedImage;
import java.awt.image.ColorConvertOp;
import java.io.IOException;
import java.util.ArrayList;
import java.awt.Color;
import temporal.puntual.AritmeticaDeImagen;

/**
 * Clase del algoritmo de Harris
 * @author sdelaot
 */
public class Harris {
    /**
     * Crea el objeto Harris
     */
    public Harris() {
    
    }
    /************************ INICIA ALGORITMO HARRIS ***********************/
    /**
     * Detector de esquina Harris
     * @param image la imagen que se procesa
     * 
     * @param k la constante o coeficiente de harris
     * @return devuelve R
     * @throws IOException por si ocurre un erros de flujo de entrada o salida 
     */
    public double[][] findR(BufferedImage image, double k) 
            throws IOException {
        AritmeticaDeImagen aritmetica = new AritmeticaDeImagen();
        int alto = image.getHeight();
        int ancho = image.getWidth();
        image = getGreyImage(image);

        //  Gx Gradiente de X
        double [][] kernelX = this.GausX(0.5);
        //  Gy Gradiente de Y
        double [][] kernelY = this.GausY(0.5);

        //  Ix 
        double [][] I_x = 
                this.convolution(
                        transformImageToArray(image), 
                        kernelY);
        //  Iy
        double [][] I_y = 
                this.convolution(
                        transformImageToArray(image), 
                        kernelX);

        // Ix^2 = Ix * Ix
        //double [][] I_x2 = new double[I_x.length][I_x[0].length];
        double [][] I_x2 = 
                aritmetica.multiplicarImagenes(
                        I_x, 1, I_x, 1, false);
//        for(int i = 0; i<I_x.length; i++) {
//            for(int j = 0; j<I_x[0].length; j++) {
//                I_x2[i][j] = I_x[i][j]*I_x[i][j];
//            }
//        }
        // Iy^2 = Iy * Iy
        //double [][] I_y2 = new double[I_y.length][I_y[0].length];
        double [][] I_y2 = 
                aritmetica.multiplicarImagenes(
                        I_y, 1, I_y, 1, false);
//        for(int i = 0; i<I_x.length; i++) {
//            for(int j = 0; j<I_x[0].length; j++) {
//                I_y2[i][j] = I_y[i][j]*I_y[i][j];
//            }
//        }
        // Ixy = Ix * Iy
        //double [][] I_xy = new double[I_y.length][I_y[0].length];
        double [][] I_xy = 
                aritmetica.multiplicarImagenes(
                        I_x, 1, I_y, 1, false);
//        for(int i = 0; i<I_x.length; i++) {
//            for(int j = 0; j<I_x[0].length; j++) {
//                I_xy[i][j] = I_x[i][j]*I_y[i][j];
//            }
//        }
        //Filtrar con diferente sigma
        double [][] kernel = this.createKernel(1);

        //Sx2 = convolution(I_x2)
        double [][] S_x2 = this.convolution(I_x2, kernel);

        //Sy2 = convolution(I_y2)
        double [][] S_y2 = this.convolution(I_y2, kernel);

        //Sxy = = convolution(I_xy)
        double [][] S_xy = this.convolution(I_xy, kernel);

        double low = 0.5;
        /*
         Se crea la matriz R
         */
        double [][] R = new double[alto][ancho];
        double maxR = 0;

        for(int i = 0; i<alto; i++) {
            for(int j = 0; j<ancho; j++) {
                double trace = S_x2[i][j] + S_y2[i][j];
                double det = S_x2[i][j]*S_y2[i][j] - S_xy[i][j]*S_xy[i][j];
                double r = det - k*trace*trace;
                R[i][j] = r;
            }
        }

        return R;
    }
    
    /**
     * Convertir la imagen a gris
     * @param image la imagen a corregir
     * @return devuelve la imagen convertida
     * @throws IOException excepcion de salida
     */
    public BufferedImage getGreyImage(BufferedImage image) throws IOException {
        try {

            int width = image.getWidth();
            int height = image.getHeight();
            for(int i = 0; i<width; i++) {
                for(int j = 0; j<height; j++) {
                    Color c = new Color(image.getRGB(i, j));

                    int red   = (int) (c.getRed()   * 0.2126);
                    int green = (int) (c.getGreen() * 0.7152);
                    int blue  = (int) (c.getBlue()  * 0.0722);
                    int gris = red+green+blue;
                    Color newColor = new Color(gris, gris, gris);
                    image.setRGB(i, j, newColor.getRGB());
                    }
                }
        } catch (Exception e) {
            e.getMessage();
        }
        return image;
    }
    
    /**
     * Obtiene DoGX Obtiene la mascara de gauss de X
     * @param sigma el valor de la desviacion estandar
     * @return devuelve el kernel creado
     */
    public double[][] GausX(double sigma) {
        double[] filter = {-1, 0, 1};
        double[][] kernel = createKernel(sigma);
        for(int i = 0; i<kernel.length; i++) {
            for(int j = 0; j<kernel.length; j++) {
                kernel[i][j] *= filter[j];
                }
            }
        return kernel;
    }
    
    /**
     * Creación del núcleo
     * @param sigma la desviacion estandar
     * @return devuelve el kernel
     */
    public double[][] createKernel(double sigma) {
        int W = 3;
        double[][] kernel = new double[W][W];

        double mean = W/2;
        double sum = 0;

        for(int x = 0; x<W; ++x) {
            for(int y = 0; y<W; ++y) {
                kernel[x][y] = (Math.exp(-0.5*(
                                Math.pow((x-mean)/sigma, 2.0)+
                                Math.pow((y-mean)/sigma, 2.0)))
                        /(2*Math.PI*sigma*sigma));

                sum += kernel[x][y];
                }
            }

        for(int i = 0; i<W; i++) {
            for(int j = 0; j<W; j++) {
                kernel[i][j] /= sum;
                }
            }

        return kernel;
    }
    
    /**
     * Obtiene DoGY Crea el kernel para el gradiente de Y
     * @param sigma la desviacion estandar
     * @return devuelve el kernel de convolucion
     */
    public double[][] GausY(double sigma) {
        double[] filter = {-1, 0, 1};
        double[][] kernel = createKernel(sigma);
        for(int i = 0; i<kernel.length; i++) {
            for(int j = 0; j<kernel.length; j++) {
                kernel[i][j] *= filter[i];
                }
            }
        return kernel;
    }
    
    /**
     * Convertir imagen en una matriz double
     * @param bufferedImage La imagen a convertir
     * @return por si ocurre un error de lectura de la imagen
     */
    public double [][] transformImageToArray(BufferedImage bufferedImage) {
        int width = bufferedImage.getWidth();
        int height = bufferedImage.getHeight();

        double[][] image = new double[height][width];

        for(int i = 0; i<height; i++) {
            for(int j = 0; j<width; j++) {
                Color color = new Color(bufferedImage.getRGB(j, i));
                image[i][j] = color.getRed();
                }
            }
        return image;
    }
    
    /****************************** CONVOLUCION *****************************/
    
    /**
     * Convolución, Ix, Iy
     * @param original la image original
     * @param kernel la mascara de convolucion
     * @return devuelve la matriz de imagen convolucionada
     */
    public double [][] convolution(double[][] original, 
            double [][] kernel) {
        int additional = kernel.length-1;
        int semiadittional = additional/2;
        // AQUI PUEDE FALLAR
        double[][] newMatrix = 
                new double[original[0].length+additional]
                          [original.length+additional];
        //Big matrix
        for(int x=0, i=semiadittional+x; x<original[0].length; x++, i++) {
            for(int y=0, j= semiadittional+y; y<original.length; y++, j++) {
                newMatrix[i][j] = original[y][x];
                }
            }

        double[][] result = new double[original.length][original[0].length];

        for(int x=0, i=semiadittional+x; x<original[0].length; x++, i++) {
            for(int y=0, j=semiadittional+y; y<original.length; y++, j++) {
                double[][] sub = getSubMatrix(
                            i-semiadittional, 
                            j-semiadittional, 
                            i+semiadittional, 
                            j+semiadittional, 
                            newMatrix);
                result[y][x] = convolutionElements(sub, kernel);
                }
            }

        return result;
    }
    
    /**
     * Devuelve la sub matriz que necesita el proceso de convolucion
     * 
     * @param x0 coordenada inicial en x
     * @param y0 coordenada inicial en y
     * @param xN coordenada final en x
     * @param yN coordenada final en y
     * @param original la imagen original (matriz double [][])
     * 
     * @return devuelve la matriz double [][] que contiene la sub imagen para
     * realizar la convolucion (result)
     */
    public double[][] getSubMatrix(int x0, int y0, int xN, int yN, 
            double[][] original) {
        double[][] result = new double[xN-x0+1][yN-y0+1];

        for(int x = 0, i = x0+x; x<result[0].length; x++, i++) {
            for(int y = 0, j = y0+y; y<result.length; y++, j++) {
                result[x][y] = original[i][j];
                }
            }
        return result;
    }
    
    /**
     * Convoluciona los elementos matriciales, la sub imagen (matrix) y la 
     * mascara (kernel)
     * @param matrix la matriz que tiene un pedazo de imagen
     * @param kernel ma mascara de convolucion
     * @return devuelve el resultado de la convolucion de los elementos
     */
    public int convolutionElements(double[][] matrix, 
            double[][] kernel) {

        int result = 0;
        for(int i = 0; i<kernel[0].length; i++) {
            for(int j = 0; j<kernel.length; j++) {
                result += matrix[i][j]*kernel[i][j];
                }
            }
        return result;
    }
    /************************** TERMINA CONVOLUCION *************************/
    
    /**
     * Normalizar la matriz
     * @param matrix la matriz que se normaliza
     * 
     * @return devuelve la matriz normalizada
     */
    public double[][] normalize(double[][] matrix) {
        double maxR = 0;
        for(int i = 0; i<matrix.length; i++) {
            for(int j = 0; j<matrix[0].length; j++) {
                if(maxR<matrix[i][j]) {
                    maxR = matrix[i][j];
                    }
                }
            }

        for(int i = 0; i<matrix.length; i++) {
            for(int j = 0; j<matrix[0].length; j++) {
                matrix[i][j] /= maxR;
                }
            }

        return matrix;
    }
    
     /**
      * Convertir a rango 0-1
      * @param R la matriz que se convertira en binaria
      * @return devuelve R en binario (0 o R)
      */
    public double[][] threshhold(double[][] R) {
        double thresh = 0.001;
        for(int i = 0; i<R.length; i++) {
            for(int j = 0; j<R[0].length; j++) {
                if(R[i][j]<thresh) {
                    R[i][j] = 0;
                    }
                }
            }
        return R;
    }
    
    /**
     * Area alrededor de los puntos de interes. Encontramos el angulo y en el 
     * area del angulo 5x5 encontramos el maximo que hacemos y le asignamos el 
     * resto 0
     * @param R R es donde se tiene la informacion de el area de puntos de 
     * interes
     * @param image la imagen en donde se busca respecto de la informacion de R
     * @return devuelve los puntos de interes dentro de la imagen, cambia el 
     * color del pixel en la coordenada (X, Y) ([Y][X]) por un tono rojo.
     */
    public double[][] cornerDetector2(double[][] R, 
            BufferedImage image) {
        double max;
        int Y; // coordenada en y donde se encuentra el valor maximo
        int X; // coordenada en x donde se encuentra el valor maximo
        for(int i = 2; i<R.length-2; i++) {
            for(int j = 2; j<R[0].length-2; j++) {
                max = 0;
                Y = 0;
                X = 0;
                if(R[i][j]>0) {
                    for(int y = i-2; y<i+3; y++) {
                        for(int x = j-2; x<j+3; x++) {
                            if(max<R[y][x]) {
                                max = R[y][x];
                                Y = y;
                                X = x;
                                R[y][x] = 0;
                                }
                            else {
                                R[y][x] = 0;
                                }
                            }
                        }
                    R[Y][X] = 1;
                    Color c = new Color(255, 0, 0);
                    image.setRGB(X, Y, c.getRGB());
                    }
                }
            }

        return R;
    }
    
    /**
     * Ingresando las coordenadas de las esquinas en una matriz
     *
     * @param R contenido de los puntos de interes
     * 
     * @return devuelve las coordenadas de las equinas en un ArrayList que 
     * almacena un arreglo de Integer
     */
    public ArrayList<Integer[]> findCorners(double[][] R) {
        ArrayList<Integer[]> corners = new ArrayList<>();
        for(int i = 0; i<R.length; i++) {
            for(int j = 0; j<R[0].length; j++) {
                if(R[i][j]>0) {
                    corners.add(new Integer[]{i, j});
                    }
                }
            }
        return corners;
    }
    
    /**
     * Area de brillo de píxeles alrededor de puntos de interés para el 
     * descriptor
     * @param corners esquinas
     * @param image la imagen
     * @param descriptorFactor el factor descriptor
     * @return devuelve la lista de esquinas
     */
    public ArrayList<Double[]> areaAroundInterstingPoints2(
            ArrayList<Integer[]> corners, 
            BufferedImage image, int descriptorFactor) {

        ArrayList<Double[]> descriptors = new ArrayList<Double[]>();
        int descriptorFactor2 = descriptorFactor*descriptorFactor;
        int semiFactor = descriptorFactor/2;
        int semiFactorIncr = semiFactor+1;
        semiFactor = 0-semiFactor;

        for(Integer[] corner : corners) {
            int r = corner[0], c = corner[1];
            int index = 0;
            Double[] descriptor = new Double[descriptorFactor2];
            for(int x = semiFactor; x<semiFactorIncr; x++) {
                for(int y = semiFactor; y<semiFactorIncr; y++, index++) {
                    int currentR = r+x;
                    int currentC = c+y;
                    if(currentR>0&&currentR<image.getWidth() &&
                       currentC>0&&currentC<image.getHeight()) {
                        Color color = 
                                new Color(
                                        image.getRGB(currentR, currentC));
                        descriptor[index] = Double.valueOf(color.getRed());
                    }
                    else {
                        descriptor[index] = 0.0;
                    }
                }
            }
            descriptors.add(descriptor);
        }
        return descriptors;

    }
    
    /**
     * Comparar descriptores
     * @param descriptors1 descriptor uno
     * @param descriptors2 descriptor dos
     * @param descriptorFactor devuelve la comparacion
     * @return devuelve una lista de comparacion
     */
    public ArrayList<Integer[]> matchDescriptor(
            ArrayList<Double[]> descriptors1, 
            ArrayList<Double[]> descriptors2, int descriptorFactor) {

        Double[][] comparison = 
                new Double[descriptors1.size()][descriptors2.size()];
        ArrayList<Integer[]> pairs = new ArrayList<>();
        int descriptorFactor2 = descriptorFactor*descriptorFactor;

        for(int r = 0; r<comparison.length; r++) {
            for(int c = 0; c<comparison[0].length; c++) {
                float sqrt = 0;
                for(int index = 0; index<descriptorFactor2; index++) {
                    Double delta = 
                            descriptors1.get(r)[index] -
                            descriptors2.get(c)[index];
                    sqrt += delta*delta;
                }
                comparison[r][c] = Math.sqrt(sqrt);
            }
        }

        for(int r = 0; r<comparison.length; r++) {
            Double[] currentMin = 
                    new Double[]{Double.MAX_VALUE, Double.MAX_VALUE};
            int minId = 0;
            for(int c = 0; c<comparison[0].length; c++) {
                if(comparison[r][c]<currentMin[0]) {
                    currentMin[1] = currentMin[0];
                    currentMin[0] = comparison[r][c];
                    minId = c;
                }
            }
            if(currentMin[0]/currentMin[1]<0.8f) {
                for(int i = 0; i<comparison[0].length; i++) {
                    comparison[i][minId] = Double.POSITIVE_INFINITY;
                }
                pairs.add(new Integer[]{r, minId});
            }
        }

        return pairs;
    }
    
    /**
     * Dibuja una línea entre las esquinas que coincidieron
     *
     * @param image1 la imagen uno
     * @param image2 la imagen dos
     * @param pairs los pares
     * @param coordinate1 las coordenadas uno
     * @param coordinate2 las coordenadas dos
     * @return devuelve la imagen con una linea dibujada
     */
    public BufferedImage drawMatches(BufferedImage image1, 
            BufferedImage image2, 
            ArrayList<Integer[]> pairs, 
            ArrayList<Integer[]> coordinate1, 
            ArrayList<Integer[]> coordinate2) {

        BufferedImage image = new BufferedImage(
                image1.getWidth()+image2.getWidth(), 
                Math.max(image1.getHeight(), 
                        image2.getHeight()), BufferedImage.TYPE_INT_RGB);

        for(int x = 0; x<image1.getWidth(); x++) {
            for(int y = 0; y<image1.getHeight(); y++) {
                image.setRGB(x, y, image1.getRGB(x, y));
            }
        }

        for(int x=image2.getWidth(), x0=0; x0<image2.getWidth(); x++, x0++) {
            for(int y = 0; y<image2.getHeight(); y++) {
                image.setRGB(x, y, image2.getRGB(x0, y));
                }
            }

        Graphics2D g2d = image.createGraphics();
        Color color = new Color(0, 255, 0);

        for(Integer[] pair : pairs) {
            Integer[] c1 = coordinate1.get(pair[0]);
            Integer[] c2 = coordinate2.get(pair[1]);

            Point p1 = new Point(c1[1], c1[0]);
            Point p2 = new Point(c2[1]+image1.getWidth(), c2[0]);
            g2d.setColor(color);
            g2d.drawLine(p1.x, p1.y, p2.x, p2.y);

            image.setRGB(p1.x, p1.y, color.getRGB());
            image.setRGB(p2.x, p2.y, color.getRGB());
//            g2d.fillOval(p1.x-2,p1.y-2,5,5);
//            g2d.fillOval(p2.x-2,p2.y-2,5,5);
        }

        return image;
    }
    /**
     * Dibuja una línea entre las esquinas que coincidieron
     *
     * @param image1 la imagen
     * @param coordinate1 las coordenadas
     * @return devuelve la imagen con la linea dibujada
     */
    public BufferedImage drawImage(BufferedImage image1,  
            ArrayList<Integer[]> coordinate1) {
        int ancho = image1.getWidth();
        int alto = image1.getHeight();

        BufferedImage image = new BufferedImage(
                ancho, 
                alto, BufferedImage.TYPE_INT_RGB);

        for(int x = 0; x<image1.getWidth(); x++) {
            for(int y = 0; y<image1.getHeight(); y++) {
                image.setRGB(x, y, image1.getRGB(x, y));
            }
        }
        
        Graphics2D g2d = image.createGraphics();
        Color color = new Color(255, 0, 0);
        // DEBUG
        //.out.println("No. Puntos " + coordinate1.size());
        int elPunto = 0;
        for(Integer[] pair : coordinate1) {
            Integer[] c1 = coordinate1.get(pair[0]);

            Point p1 = new Point(c1[1], c1[0]);
            // DEBUG
            //.out.println("P(" + elPunto + ")\t" + p1 );
                                        // + " " + color.getRGB());
            elPunto++;
            g2d.setColor(color);
            //g2d.drawLine(p1.x, p1.y, p1.x, p1.y);
            int x = p1.x;
            int y = p1.y;

            if(x-1>=0) {
                image.setRGB(x-1, y  , color.getRGB());
                // DEBUG
                //System.out.println("1");
                }
            if(y-1>=0) {
                image.setRGB(x  , y-1, color.getRGB());
                // DEBUG
                //System.out.println("2");
                }
            image.setRGB(x  , y  , color.getRGB());
            //System.out.println("0");
            if(y+1<alto) {
                image.setRGB(x  , y+1, color.getRGB());
                // DEBUG
                //System.out.println("3");
                }
            if(x+1<ancho) {
                image.setRGB(x+1, y  , color.getRGB());
                // DEBUG
                //System.out.println("4");
                }
//            try {
//                g2d.fillOval(x, y, 4, 4);
//                Thread.sleep(5);
//            } catch(InterruptedException ie) {} 1692 1795
            }
        // DEBUG
        //System.out.println("No. Puntos " + coordinate1.size());
        return image;
    }
    /*********************** TERMINA ALGORITMO HARRIS ***********************/
    
    /**
     * Obtener una imagen bw
     * @param img la imagen en buffer
     * @return devuelve un Buffered Image
     */
    public static BufferedImage convertToGrayScale(BufferedImage img) {
        ColorSpace cs = ColorSpace.getInstance(ColorSpace.CS_GRAY);
        ColorConvertOp op = new ColorConvertOp(cs, null);
        BufferedImage image = op.filter(img, null);
        return image;
    }

    /**
     * Convertir la imagen en una matriz de 0 y 1 para mayor precisión
     
     * @param bufferedImage la imagen
     * @return la imagen binaria
     */
    public double[][] getArrayFromBinaryImage(BufferedImage bufferedImage) {
        BufferedImage source;
        double[][] resultImage = new double[0][0];
        try {
            source = bufferedImage;
            int rowCount = source.getHeight();
            int colCount = source.getWidth();
            resultImage = new double[rowCount][colCount];
            for(int i = 0; i<rowCount; i++) {
                for(int j = 0; j<colCount; j++) {
                    Color pixelColor = new Color(source.getRGB(j, i));
                    int red = (int) (pixelColor.getRed());
                    int green = (int) (pixelColor.getGreen());
                    int blue = (int) (pixelColor.getBlue());
                    /*
                     240 y superior es blanco
                     */
                    if(red>240&&green>240&&blue>240) {
                        resultImage[i][j] = 1;
                    }
                    else {
                        resultImage[i][j] = 0;
                    }
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }

        return resultImage;
    }

    /**
     * Visualización de la matriz
     *
     * @param matrix la matris a imprimir 
     */
    public void printMatrix(double[][] matrix) {
        String result = "";
        for(int i = 0; i<getRowCount(matrix); i++) {
            for(int j = 0; j<getColumnCount(matrix); j++) {
                result += matrix[i][j]+" ";
            }
            result += "\n";
        }
        // DEBUG
        //System.out.println(result);
    }

    /**
     * Devuelve el número de columnas.
     *
     * @param matrix la matris
     * @return devuelve el conteo de columnas
     */
    private int getColumnCount(double[][] matrix) {
        return matrix[0].length;
    }

    /**
     * Devuelve el número de filas
     *
     * @param matrix la matriz
     * @return devuelve la altura
     */
    private int getRowCount(double[][] matrix) {
        return matrix.length;
    }

    /**
     * Obtener una imagen de una matriz
     *
     * @param matrix la matriz
     * @return devuelve la imagen en BufferedImage
     */
    public BufferedImage drawImage(double[][] matrix) {

        BufferedImage img = new BufferedImage(
                matrix.length, 
                matrix[0].length, 
                BufferedImage.TYPE_INT_RGB);

        for(int i = 0; i<matrix.length; i++) {
            for(int j = 0; j<matrix[0].length; j++) {
                int pixel = (int) matrix[i][j];
                img.setRGB(i, j, pixel);
            }
        }

        return img;
    }
    /**
     * Convierte una tripleta rgb en gris
     * @param rgb el rgb
     * @return devuelve el valor de gris
     */
    public static int getGrayScale(int rgb) {
        int r = (rgb>>16)&0xff;
        int g = (rgb>>8)&0xff;
        int b = (rgb)&0xff;

        int gray = (int) (0.2126*r+0.7152*g+0.0722*b);

        return gray;
    }
    


    /**
     * Derivación de matriz de descriptores
     *
     * @param descriptor el descriptor
     */
    public void printPatch(ArrayList<Double[]> descriptor) {
        for(int i = 0; i<descriptor.size(); i++) {
            for(int z = 0; z<descriptor.get(i).length; z++) {
                System.out.print(descriptor.get(i)[z]+" ");
            }
            System.out.println();
        }
    }

    /**
     * Salida de matriz angular
     * @param descriptor el descriptor
     */
    public void printCorners(ArrayList<Integer[]> descriptor) {
        for(int i = 0; i<descriptor.size(); i++) {
            for(int z = 0; z<descriptor.get(i).length; z++) {
                System.out.print(descriptor.get(i)[z]+" ");
                }
            System.out.println();
            }
    }
}
