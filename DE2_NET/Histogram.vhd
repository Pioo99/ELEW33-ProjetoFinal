library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity histogram_avalon is
  port (
    clk             : in  std_logic;
    rst             : in  std_logic;
    -- A interface Avalon usará:
    -- - avalon_writedata para receber o valor do pixel (nos 8 bits menos significativos)
    -- - avalon_write indica que um novo pixel foi enviado
    -- - avalon_address é usado na operação de leitura para selecionar qual bin do histograma retornar
    avalon_address   : in  std_logic_vector(7 downto 0);
    avalon_write     : in  std_logic;
    avalon_writedata : in  std_logic_vector(15 downto 0);
    avalon_read      : in  std_logic;  -- Pode ser usado se desejar controle adicional na leitura
    avalon_readdata  : out std_logic_vector(15 downto 0)
  );
end histogram_avalon;

architecture behavioral of histogram_avalon is
  -- Vetor para armazenar o histograma: 256 bins, cada um com 16 bits para contagem
  type histo_array is array (0 to 255) of unsigned(15 downto 0);
  signal histogram : histo_array := (others => (others => '0'));
begin

  ----------------------------------------------------------------------------
  -- Processo de atualização do histograma:
  -- Toda vez que avalon_write é ativo, interpreta os 8 bits menos significativos
  -- de avalon_writedata como o valor do pixel (0 a 255) e incrementa o contador correspondente.
  ----------------------------------------------------------------------------
  process(clk)
    variable pixel_val : integer;
  begin
    if rising_edge(clk) then
      if rst = '1' then
        for i in 0 to 255 loop
          histogram(i) <= (others => '0');
        end loop;
      else
        if avalon_write = '1' then
          -- Converte os 8 bits menos significativos para um inteiro (valor do pixel)
          pixel_val := to_integer(unsigned(avalon_writedata(7 downto 0)));
          histogram(pixel_val) <= histogram(pixel_val) + 1;
        end if;
      end if;
    end if;
  end process;

  ----------------------------------------------------------------------------
  -- Leitura via Avalon-MM:
  -- Quando o mestre (por exemplo, o Nios II) realiza uma leitura, o endereço
  -- (avalon_address) seleciona qual bin do histograma será retornado.
  ----------------------------------------------------------------------------
  avalon_readdata <= std_logic_vector(histogram(to_integer(unsigned(avalon_address))));

end behavioral;
