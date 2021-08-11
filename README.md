# INE5425 - Otimizador

# Introdução

Este projeto foi desenvolvido como parte da disciplina de Modelagem e Simulação do Curso de Ciências da Computação na Universidade Federal de Santa Catarina do semestre 2021/1.

Essa aplicação pode ser utilizada para otimizar variáveis de controle de modelos de simulação, sendo aplicadas restrições e tentando máximizar/minimizar uma função objetivo. Ele é construído para funcionar com o simulador [GenESys](https://github.com/rlcancian/2019_2022_GenESyS) mas com alguns ajustes, em especial nos parsers de modelos e relatórios, pode ser utilizado com outros simuladores.

# Como utilizar

Após clonar o repositório e o repositório do [GenESys](https://github.com/rlcancian/2019_2022_GenESyS), você antes de tudo precisará compilar o simulador. Para isso vá ao diretório do GenESys e execute:

> make QMAKE=qmake build

Em seguida você vai precisar ir para o diretório do otimizador e compilá-lo. Execute os seguintes comandos:

> cmake -S src -B build
> cmake --build build

Por fim, você pode executar o projeto de teste e analisá-lo pra entender melhor como montar seu arquivo de configuração. Lembre de configurar o caminho completo para o otimizador e para o modelo a ser otimizado antes de executar o seguinte comando:

> ./build/otimizador -c sample_data/process_test.txt -v 2

Fique atento que você sempre precisará inserir a flag `-c` seguida do caminho relativo para o arquivo de configuração da otimização, a verbosidade pode ser alterada com `-v` mas é colocado por padrão como 2. Ao final da execução você poderá conferir na pasta `build/projects` os resultados das suas simulações, incluindo modelos de entrada e resultados do GenESys. Caso você queira salvar os resultados finais do seu projeto sem todos os logs execute:

> ./build/otimizador -c sample_data/process_test.txt -v 0 > results.txt

# Modelo genérico para GenESys

Para utilizar o otimizador é necessário que você crie um modelo genérico que aceita especificações de modelo em linguagem de simulação, com entrada via linha de comando. Para isso você pode copiar o seguinte trecho de código e trocar pela função main de uma aplicação do GenESys:

``` cpp
if (argc != 2) {
		std::cout << "\033[31mInclude only one input file, with full path\033[37m\n";
		return 0;
	} else {
		if (FILE *file = fopen(argv[1], "r")) {
			fclose(file);
		} else {
			std::cout << "\033[31mCouldn't find input file\033[37m\n";
			return 0;
		}
	}

	Simulator* genesys = new Simulator();
	genesys->getTracer()->setTraceLevel(Util::TraceLevel::L5_event);
	this->setDefaultTraceHandlers(genesys->getTracer());
	this->insertFakePluginsByHand(genesys);
	Model* model = genesys->getModels()->newModel();
	model->load(argv[1]);
	ModelSimulation* sim = model->getSimulation();
	sim->start();

	return 0;
```

Lembre de configurar o arquivo `Traits.h` também para executar essa implementação.

# Aviso

Antes de você usar o otimizador é recomendável testar se seu modelo está sendo aceito pelo GenESys, para isso compile o modelo generico e execute ele com o modelo em linguagem de simulação como entrada.