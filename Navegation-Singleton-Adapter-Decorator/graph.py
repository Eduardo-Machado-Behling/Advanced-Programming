import os
import matplotlib.pyplot as plt
import matplotlib.ticker as tic
from matplotlib.axes import Axes
import seaborn as sns
from pathlib import Path
import pandas as pd

from typing import *

UUID_TYPE_TRANSLATION = {
    0: "Point",
    1: "Line",
    2: "Polygon"
}


def readLog() -> Union[pd.DataFrame, None]:
    df = None
    for parent in ['.', 'Debug', 'Release']:
        par = Path(parent)
        if os.path.exists(par / 'data.csv'):
            df = pd.read_csv(par / 'data.csv')
            break

    return df


def drawClicks(df: pd.DataFrame, ax: Axes) -> None:
    geometries = df[df['uuidType'] > -
                    1]['uuidType'].apply(lambda x: UUID_TYPE_TRANSLATION[x])
    count = {
        'Point': 0,
        'Line': 0,
        'Polygon': 0
    }

    countDf = geometries.value_counts()

    for k in count.keys():
        if k in countDf.index.values:
            count[k] += countDf[k]

    count = pd.Series(count)
    sns.barplot(x=count.index, y=count, ax=ax, zorder=2)
    ax.yaxis.set_major_locator(tic.MaxNLocator(integer=True))
    ax.set_xticks(list(UUID_TYPE_TRANSLATION.values()))

    ax.set_xlabel("Entity")
    ax.set_ylabel("Clicks")
    ax.set_title("Mouse Clicks per Entity")
    ax.grid(True)


def drawMouse(df: pd.DataFrame, ax: Axes) -> None:
    melted = df.melt(
        id_vars=['time'],
        value_vars=['mouseX', 'mouseY'],
        value_name='Coord',
        var_name='Axis'
    )

    melted['Axis'] = melted['Axis'].str.replace('mouse', '')
    sns.lineplot(data=melted, x='time', y='Coord', hue='Axis', ax=ax)
    ax.grid(True)

    ax.set_xlabel("Execution time (s)")
    ax.set_ylabel("Coordinate (Pixels)")
    ax.set_title("Mouse Position x Execution Time")


def drawFps(df: pd.DataFrame, axes: List[Axes]) -> None:
    ax = axes[0]
    sns.lineplot(data=df, x='time', y='fps', ax=ax)
    ax.grid(True)

    ax.set_xlabel("Execution time (s)")
    ax.set_ylabel("Frames Per Second (CPU-Side)")
    ax.set_title("FPS x Execution Time")

    ax = axes[1]
    reverse = False
    if (len(df['entities'].unique()) > 1):
        sns.lineplot(data=df, x='entities', y='fps', ax=ax)
    else:
        sns.boxplot(data=df, x='fps', y='entities', ax=ax, orient='y')
        reverse = True
    ax.grid(True)

    if reverse:
        ax.set_xlabel("Frames Per Second (CPU-Side)")
        ax.set_ylabel("Entity Amount")
    else:
        ax.set_xlabel("Entity Amount")
        ax.set_ylabel("Frames Per Second (CPU-Side)")
    ax.set_title("FPS x Entity Amount")


def drawDrawcalls(df: pd.DataFrame, ax: Axes) -> None:
    if (len(df['entities'].unique()) > 1):
        sns.lineplot(data=df, x='entities', y='drawCalls', ax=ax)
    else:
        sns.scatterplot(data=df, x='entities', y='drawCalls', zorder=2, ax=ax)
    ax.grid(True)

    ax.xaxis.set_major_locator(tic.MaxNLocator(integer=True))
    ax.yaxis.set_major_locator(tic.MaxNLocator(integer=True))
    ax.set_xlabel("Entity Amount")
    ax.set_ylabel("OpenGL Drawcalls")
    ax.set_title("Drawcalls x Entity Amount")


def drawEntity(df: pd.DataFrame, ax: Axes) -> None:
    melted = df.melt(
        id_vars=['time'],
        value_vars=['pointAmount', 'linesAmount', 'polyAmount'],
        value_name='Amount',
        var_name='Geometry'
    )

    melted['Geometry'] = melted['Geometry'].str.replace(
        r'Amount', '').str.replace('s', '').str.title()

    sns.barplot(data=melted, x='Geometry', y='Amount', ax=ax, zorder=2)
    ax.yaxis.set_major_locator(tic.MaxNLocator(integer=True))
    if (melted['Amount'].max() == 0):
        ax.xaxis.set_visible(False)
    else:
        for container in ax.containers:
            ax.bar_label(container, fmt='%d')

        lim = ax.get_ylim()
        ax.set_ylim(lim[0], lim[1] + 2)

    ax.set_xlabel("Entity")
    ax.set_ylabel("Amount")
    ax.set_title("Entity Type Distribution")
    ax.grid(True)


def DrawEngineLog(df: pd.DataFrame) -> None:
    fig, axes = plt.subplots(3, 2, figsize=(18, 20))
    drawMouse(df, axes[0, 0])
    drawClicks(df, axes[0, 1])
    drawFps(df, axes[1])
    drawEntity(df, axes[2, 0])
    drawDrawcalls(df, axes[2, 1])

    fig.tight_layout()
    fig.savefig("EngineMetrics.png")


def DrawNavegation(df: pd.DataFrame) -> None:
    fig, axes = plt.subplots(2, 2, figsize=(18, 20))

    df['config'] = df['rows'].astype(str) + 'x' + df['cols'].astype(str) + '|' + df['pathAmount'].astype(str) + '/' + df['obsAmount'].astype(str)

    dfAll= df.groupby(['config', 'sample', 'run']).agg(
        {
            'pathTime': 'sum',
            'pathDist': 'sum',
            'obsAmount': 'mean',
            'pathAmount': 'mean',
            'rows': 'mean',
            'cols': 'mean'
        }
    )
    dfAll.reset_index(inplace=True)

    dfAll['pathAmount'] = dfAll['pathAmount'].astype(int)
    dfAll['obsAmount'] = dfAll['obsAmount'].astype(int)
    dfAll['rows'] = dfAll['rows'].astype(int)
    dfAll['cols'] = dfAll['cols'].astype(int)
    dfAll['pathTime'] /= 1e6

    dfAll['grid'] = dfAll['rows'].astype(str) + 'x' + dfAll['cols'].astype(str)

    dfFrame = dfAll[dfAll['rows'] == 64]

    ax = axes[0,0]
    sns.lineplot(data=dfFrame, x='pathAmount', y='pathTime', hue='obsAmount', ax=ax)
    ax.grid(True)
    ax.set_xticks(sorted(dfFrame['pathAmount'].unique()))
    ax.set_xlabel("Quantidade de Caminhos")
    ax.set_ylabel("Tempo Dijkstra Frame (ms)")
    ax.legend().set_title("Qtde de Obstaculos:")

    ax = axes[0, 1]
    sns.lineplot(data=dfFrame, x='obsAmount', y='pathTime', hue='pathAmount', ax=ax)
    ax.grid(True)
    ax.set_xticks(sorted(dfFrame['obsAmount'].unique()))
    ax.set_xlabel("Quantidade de Obstaculos")
    ax.set_ylabel("Tempo Dijkstra Frame (ms)")
    ax.legend().set_title("Qtde de Caminhos:")

    ax = axes[1, 0]
    sns.lineplot(data=dfAll[dfAll['pathAmount'] == 64], x='obsAmount', y='pathTime', hue='grid', ax=axes[1,0])
    ax.grid(True)
    ax.set_xticks(sorted(dfFrame['obsAmount'].unique()))
    ax.set_xlabel("Quantidade de Obstaculos")
    ax.set_ylabel("Tempo Dijkstra Frame (ms)")
    ax.legend().set_title("Grid:")
    ax.set_title("Quantidade de Caminhos = 64")

    ax = axes[1, 1]
    sns.lineplot(data=dfAll, x='pathDist', y='pathTime', hue='grid', ax=axes[1,1])
    ax.grid(True)
    ax.set_xlabel("Soma da Distâncias dos Caminhos:")
    ax.set_ylabel("Tempo Dijkstra Frame (ms)")
    ax.legend().set_title("Grid:")


    fig.tight_layout()
    fig.savefig("Navegation.png")


def main() -> None:
    df = readLog()
    if df is None:
        return

    DrawNavegation(df)

    plt.tight_layout()
    plt.show()

    print(df.head())


if __name__ == '__main__':
    main()
