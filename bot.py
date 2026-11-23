# Bot Discord « strlen » : quand on le @ en répondant à un message, il répond
# avec la longueur de ce message, calculée par ft_strlen (libstrlen.so, en C).
#
# Prérequis :
#   pip install discord.py
#   cc -shared -fPIC libstrlen.c -o libstrlen.so
#   DISCORD_BOT_TOKEN='...' python3 bot.py

import ctypes
import os

import discord

LIB_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "libstrlen.so")
LIB = ctypes.CDLL(LIB_PATH)
LIB.ft_strlen.argtypes = [ctypes.c_char_p]
LIB.ft_strlen.restype = ctypes.c_int


def c_strlen(text: str) -> int:
    # Longueur en octets UTF-8, comme strlen : un « é » compte pour 2.
    return LIB.ft_strlen(text.encode("utf-8"))


intents = discord.Intents.default()
intents.message_content = True  # nécessaire pour lire le texte du message parent
client = discord.Client(intents=intents)


@client.event
async def on_message(message: discord.Message):
    if message.author.bot or client.user is None:
        return
    mentioned_ids = [user.id for user in message.mentions]
    if client.user.id not in mentioned_ids or message.reference is None:
        return
    parent = message.reference.resolved
    if parent is None:
        parent = await message.channel.fetch_message(message.reference.message_id)
    if not parent.content:
        await message.reply("Le message auquel tu réponds n'a pas de texte.")
        return
    await message.reply(f"La longueur du message est : {c_strlen(parent.content)} caractères")


token = os.environ.get("DISCORD_BOT_TOKEN")
if not token:
    raise SystemExit("DISCORD_BOT_TOKEN n'est pas défini")
client.run(token)
